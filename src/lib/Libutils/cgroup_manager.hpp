// cgroup_manager.hpp

#pragma once
#include <string>
#include <memory>
#include <filesystem>

class ICgroupManager {
public:
    virtual ~ICgroupManager() = default;
    virtual void setup_job_cgroup(int job_id, const std::string& aggregate_cpuset) = 0;
    virtual void migrate_pid(int job_id, pid_t pid) = 0;
    virtual void cleanup_job_cgroup(int job_id) = 0; 
};

// The No-Op implementation for environments where cgroups are not available or desired
class NullCgroupManager : public ICgroupManager {
    public:
        void setup_job_cgroup(int /* job_id */, const std::string& /* aggregate_cpuset */) override {};
        void migrate_pid(int /*job_id */, pid_t /*pid*/) override {};
        void cleanup_job_cgroup(int /* job_id */) override {};
};

class CgroupManagerFactory {
public:
    static std::unique_ptr<ICgroupManager> create(const std::string &type);
};
