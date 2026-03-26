/*
 * torque-NG: Next Generation Resource Manager
 *
 * Copyright (c) 2026 Kenneth Nielson.
 * Portions Copyright (c) 1999-2000 Veridian Information Solutions, Inc.
 * Licensed under the OpenPBS v2.3 Software License.
 */

#pragma once

#include "torque_ng.pb.h"
#include <string>
#include <string_view>
#include <memory>
#include <vector>

namespace torque_ng {

class JobEntity {
public:
    explicit JobEntity(std::string_view job_id);
    ~JobEntity() = default;

    // Identity and State
    void set_job_owner(std::string_view owner);
    void set_job_state(torque_ng::Job::JobState state, torque_ng::Job::JobSubState sub_state = torque_ng::Job::SUB_NONE);
    void set_interactive(bool interactive);

    // Resource Management
    void set_resources_used(uint64_t mem, uint64_t vmem, double walltime);
    void set_resource_limits(uint64_t mem, uint64_t vmem, uint32_t nodes, uint32_t procs, double walltime);

    // Execution Details
    void set_execution_info(std::string_view host, int32_t exit_status);
    
    // Timestamps
    void set_timestamps(uint64_t qtime, uint64_t mtime);

    // Metadata
    void add_attribute(std::string_view key, std::string_view value);
    void add_env_var(std::string_view key, std::string_view value);

    // Serialization
    std::string_view get_id() const;
    std::string serialize() const;
    static std::unique_ptr<JobEntity> deserialize(const std::string& data);

private:
    std::unique_ptr<torque_ng::JobStatusUpdate> m_msg;
};

} // namespace torque_ng