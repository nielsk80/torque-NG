// machine.cpp
#include "machine.hpp"
#include "factory.hpp"
#include "cgroup_manager.hpp"
#include "allocation.hpp"
#include "user_context.hpp"
#include "pbs_log.h"
#include "pbs_error.h"

#include <stdexcept>
#include <mutex>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <unistd.h>

/**
 * CgroupConfig: Helper class to handle PBS environment discovery
 * and configuration parsing.
 */
class CgroupConfig {
public:
    static std::string get_cgroup_type() {
        std::string pbs_home = "/var/spool/pbs"; // Default fallback

        // 1. Try to discover PBS_HOME from /etc/pbs.conf
        std::ifstream pbs_conf("/etc/pbs.conf");
        if (pbs_conf.is_open()) {
            std::string line;
            while (std::getline(pbs_conf, line)) {
                // Look for PBS_HOME=...
                if (line.compare(0, 9, "PBS_HOME=") == 0) {
                    pbs_home = line.substr(9);
                    // Remove quotes if present
                    pbs_home.erase(std::remove(pbs_home.begin(), pbs_home.end(), '\"'), pbs_home.end());
                    // Clean up trailing whitespace/newlines
                    size_t last = pbs_home.find_last_not_of(" \n\r\t");
                    if (last != std::string::npos)
                        pbs_home.erase(last + 1);
                    break;
                }
            }
        }

        // 2. Build path to mom_priv/config and parse for $cgroup_type
        std::string config_path = pbs_home + "/mom_priv/config";
        std::ifstream config_file(config_path);
        
        if (config_file.is_open()) {
            std::string line;
            while (std::getline(config_file, line)) {
                if (line.find("$cgroup_type") != std::string::npos) {
                    size_t last_space = line.find_last_of(" \t");
                    if (last_space != std::string::npos) {
                        std::string value = line.substr(last_space + 1);
                        // Final cleanup of the value
                        size_t last = value.find_last_not_of(" \n\r\t");
                        if (last != std::string::npos)
                            value.erase(last + 1);
                        return value;
                    }
                }
            }
        }

        return "v2"; // Default fallback if config or parameter is missing
    }
};

Machine::Machine() {
    // Determine the manager type from the config file logic above
    std::string type = CgroupConfig::get_cgroup_type();
    
    // Pass the type to the Factory (resolves "too few arguments" error)
    cgroup_mgr = CgroupManagerFactory::create(type);

    std::string msg = "Cgroup Manager initialized with type: " + type;
    log_event(PBSEVENT_DEBUG, PBS_EVENTCLASS_SERVER, "Machine", msg.c_str());
}

HardwareNode& Machine::get_topology() {
    if (!root_node) {
        root_node = TopologyFactory::build_system_tree();
        root_node->aggregate_resources();
    }
    return *root_node;
}

bool Machine::has_sufficient_resources(int cores_requested, uint64_t mem_requested) {
    auto &root = get_topology();
    return (root.available_threads >= cores_requested &&
            root.available_memory >= mem_requested);
}

void Machine::set_strategy(std::unique_ptr<PlacementStrategy> strategy) {
    current_strategy = std::move(strategy);
}

bool Machine::schedule_job(const Job& job) {
    if (!current_strategy) return false;
    
    auto& root = get_topology();
    auto selected_nodes = current_strategy->find_placement(&root, job);
        
    if (selected_nodes.size() < (size_t)job.required_cores) {
        log_event(PBSEVENT_DEBUG, PBS_EVENTCLASS_SERVER, "Machine", "Insufficient resources for placement.");
        return false;
    }

    // Capture the placement results into an allocation object.
    allocation job_alloc;
    job_alloc.assigned_nodes = selected_nodes;
    std::string cpuset_string = job_alloc.get_cpuset_string();

    // Delegate isolation container setup to the manager
    cgroup_mgr->setup_job_cgroup(job.id, cpuset_string);

    pid_t pid = fork();

    if (pid < 0) {
        log_err(PBSE_INTERNAL, __func__, ("Failed to fork for job " + std::to_string(job.id)).c_str());
        return false;
    }

    if (pid == 0) {
        // --- CHILD PROCESS ---
        // 1. Instantiate the context using the job owner's name
        // Note: You'll need to ensure the Job struct has an 'owner' field
        UserContext user(job.user); // Use the correct member name
        if (!user.exists()) {
            log_err(PBSE_INTERNAL, __func__, "Child failed: User does not exist");
            _exit(1); 
        }

        // 2. Drop privileges (initgroups -> setgid -> setuid)
        if (!user.apply_to_current_process()) {
            log_err(PBSE_INTERNAL, __func__, "Child failed: Unable to drop privileges");
            _exit(1); 
        }   

        // 3. Execution environment setup (Optional: chdir to home, set env vars)
        chdir(user.get_home_dir().c_str());      

        // The child process would exec the job's command here.
        // 4. Replace child image with the job script
        // execl("/bin/bash", "bash", job.script_path.c_str(), (char*)NULL);

        // For this example, we'll just exit successfully.
        _exit(0);
    } else {
        // --- PARENT PROCESS (MOM) ---
        
        // Move the child PID into the isolation group synchronously
        cgroup_mgr->migrate_pid(job.id, pid);

        // Update internal resource tracking
        for (auto* node : selected_nodes) {
            node->available_threads--;
        }
        root.available_threads -= job.required_cores;

        std::string msg = "Job " + std::to_string(job.id) + " successfully launched and isolated.";
        log_event(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, "Machine", msg.c_str());

        return true;
    }
}