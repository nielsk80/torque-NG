#include "cgroup_manager.hpp"
#include <fstream>
#include <pbs_log.h>
#include <pbs_error.h>
#include <stdexcept>
#include <memory>


class LinuxV2CgroupManager : public ICgroupManager {
public:
    void setup_job_cgroup(int job_id, const std::string& aggregate_cpuset) override {
        namespace fs = std::filesystem;
        const std::string routine = "LinuxV2CgroupManager::setup";
        fs::path base_path = "/sys/fs/cgroup/torque";
        fs::path job_path = base_path / ("job_" + std::to_string(job_id));

        try {
            if (!fs::exists(base_path)) {
                fs::create_directory(base_path);
                std::ofstream(base_path / "cgroup.subtree_control") << "+cpuset +memory +cpu";
            }

            fs::create_directory(job_path);
            std::ofstream(job_path / "cpuset.cpus") << aggregate_cpuset;
            std::ofstream(job_path / "cpuset.mems") << "0"; 

            // Log successful creation as a system event
            std::string msg = "Created isolation container for job " + std::to_string(job_id);
            log_event(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, "TORQUE", msg.c_str());
        }
        catch (const std::exception& e) {
            log_err(PBSE_INTERNAL, __func__, e.what());
        }  
    }

    void migrate_pid(int job_id, pid_t pid) override {
        const std::string routine = "LinuxV2CgroupManager::migrate";
        std::string path = "/sys/fs/cgroup/torque/job_" + std::to_string(job_id) + "/cgroup.procs";

        try {
            std::ofstream outfile(path);
            if (!outfile.is_open()) {
                log_err(PBSE_INTERNAL, routine, "Failed to open cgroup.procs for job " + std::to_string(job_id));
                return;
            }
            outfile << pid;
        } catch (const std::exception& e) {
            log_err(PBSE_INTERNAL, routine, e.what());
        }
    }

    void cleanup_job_cgroup(int job_id) override {
        const std::string routine = "LinuxV2CgroupManager::cleanup";
        namespace fs = std::filesystem;
        fs::path job_path = fs::path("/sys/fs/cgroup/torque") / ("job_" + std::to_string(job_id));

        try {
            if (fs::exists(job_path)) {
                fs::remove(job_path);
            }
        } catch (const std::exception& e) {
            log_err(PBSE_INTERNAL, routine, e.what());
        }
    }
};

std::unique_ptr<ICgroupManager> CgroupManagerFactory::create(const std::string &type)
{
   if (type == "v1")
   {
      log_event(PBSEVENT_DEBUG, PBS_EVENTCLASS_SERVER, "CgroupMgr", "Creating V1 Manager");
      return nullptr; // Placeholder for actual V1 implementation
                      // return std::make_unique<LinuxV1CgroupManager>();
   }
   else if (type == "none")
   {
      log_event(PBSEVENT_DEBUG, PBS_EVENTCLASS_SERVER, "CgroupMgr", "Cgroups disabled");
      return std::make_unique<NullCgroupManager>();
   }

   // Default to V2
   log_event(PBSEVENT_DEBUG, PBS_EVENTCLASS_SERVER, "CgroupMgr", "Creating V2 Manager (Default)");
   return std::make_unique<LinuxV2CgroupManager>();
}