/*
 * torque-NG: Next Generation Resource Manager
 *
 * Copyright (c) 2026 Kenneth Nielson.
 * Portions Copyright (c) 1999-2000 Veridian Information Solutions, Inc.
 * Licensed under the OpenPBS v2.3 Software License.
 */

#include "Job.hpp"

namespace torque_ng {

   // The function name must match the class name 'JobEntity'
JobEntity::JobEntity(std::string_view job_id) : m_msg(std::make_unique<torque_ng::JobStatusUpdate>()) {
    m_msg->set_job_id(std::string(job_id));
} 

void JobEntity::set_job_owner(std::string_view owner) {
    m_msg->set_job_owner(std::string(owner));
}

void JobEntity::set_job_state(torque_ng::Job::JobState state, torque_ng::Job::JobSubState sub_state) {
    m_msg->set_job_state(state);
    m_msg->set_job_substate(sub_state);
}

void JobEntity::set_interactive(bool interactive) {
    m_msg->set_is_interactive(interactive);
}

void JobEntity::set_resources_used(uint64_t mem, uint64_t vmem, double walltime) {
    auto* res = m_msg->mutable_resources_used();
    res->set_mem_bytes(mem);
    res->set_vmem_bytes(vmem);
    res->set_walltime_used(walltime);
}

void JobEntity::set_resource_limits(uint64_t mem, uint64_t vmem, uint32_t nodes, uint32_t procs, double walltime) {
    auto* lim = m_msg->mutable_limits();
    lim->set_req_mem(mem);
    lim->set_req_vmem(vmem);
    lim->set_req_nodes(nodes);
    lim->set_req_procs(procs);
    lim->set_req_walltime(walltime);
}

void JobEntity::set_execution_info(std::string_view host, int32_t exit_status) {
    m_msg->set_exec_host(std::string(host));
    m_msg->set_exit_status(exit_status);
}

void JobEntity::set_timestamps(uint64_t qtime, uint64_t mtime) {
    m_msg->set_queue_time(qtime);
    m_msg->set_modify_time(mtime);
}

void JobEntity::add_attribute(std::string_view key, std::string_view value) {
    (*m_msg->mutable_attributes())[std::string(key)] = std::string(value);
}

void JobEntity::add_env_var(std::string_view key, std::string_view value) {
    (*m_msg->mutable_environment_vars())[std::string(key)] = std::string(value);
}

std::string_view JobEntity::get_id() const {
    return m_msg->job_id();
}

std::string JobEntity::serialize() const {
    return m_msg->SerializeAsString();
}

std::unique_ptr<JobEntity> JobEntity::deserialize(const std::string& data) {
    auto job = std::make_unique<JobEntity>(""); 
    if (job->m_msg->ParseFromString(data)) {
        return job;
    }
    return nullptr; 
}

} // namespace torque_ng