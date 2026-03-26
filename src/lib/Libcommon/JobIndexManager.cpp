#include "JobIndexManager.hpp"

namespace torque_ng {

JobIndexManager::JobIndexManager(const std::string& state_file, size_t max_jobs)
    : max_jobs_(max_jobs),
      buffer_(state_file, max_jobs * (
          sizeof(Job::JobState) + 
          sizeof(int32_t) + 
          sizeof(uint32_t) + 
          sizeof(uint64_t))) {

    uint8_t* raw_base = static_cast<uint8_t*>(buffer_.get_ptr());
    size_t offset = 0;

    states_ptr_ = reinterpret_cast<Job::JobState*>(raw_base + offset);
    offset += max_jobs_ * sizeof(Job::JobState);

    priorities_ptr_ = reinterpret_cast<int32_t*>(raw_base + offset);
    offset += max_jobs_ * sizeof(int32_t);

    req_nodes_ptr_ = reinterpret_cast<uint32_t*>(raw_base + offset);
    offset += max_jobs_ * sizeof(uint32_t);

    req_mem_ptr_ = reinterpret_cast<uint64_t*>(raw_base + offset);
}

size_t JobIndexManager::get_or_create_job_index(const std::string& job_id) {
    std::lock_guard<std::mutex> lock(job_mutex_);
    auto it = id_to_index_.find(job_id);
    if (it != id_to_index_.end()) return it->second;

    if (current_count_ >= max_jobs_) {
        throw std::runtime_error("Job index capacity reached.");
    }

    size_t idx = current_count_++;
    id_to_index_[job_id] = idx;
    job_ids_.push_back(job_id);
    return idx;
}

void JobIndexManager::update_job(const Job& job_update) {
    size_t idx = get_or_create_job_index(job_update.job_id());
    
    states_ptr_[idx]     = job_update.state();
    priorities_ptr_[idx] = job_update.priority();
    
    // Extracting resource requests (example)
    // req_nodes_ptr_[idx] = ...;
    // req_mem_ptr_[idx]   = ...;

    buffer_.flush();
}

void JobIndexManager::set_job_state(const std::string& job_id, Job::JobState new_state) {
    auto it = id_to_index_.find(job_id);
    if (it != id_to_index_.end()) {
        states_ptr_[it->second] = new_state;
    }
}

} // namespace torque_ng