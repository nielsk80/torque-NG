/*
 * torque-NG: Next Generation Resource Manager
 *
 * Copyright (c) 2026 Kenneth Nielson.
 * Licensed under the OpenPBS v2.3 Software License.
 */

#pragma once

#include "MmapBuffer.hpp"
#include "torque_ng.pb.h"
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace torque_ng {

class JobIndexManager {
public:
  // We pre-allocate space for a large number of jobs (e.g., 100,000)
  JobIndexManager(const std::string &state_file, size_t max_jobs = 100000);

  // Register a new job or update an existing one
  void update_job(const Job &job_update);

  // Transition a job state (e.g., QUEUED -> RUNNING)
  void set_job_state(const std::string &job_id, Job::JobState new_state);

  // High-speed access for the scheduler
  const Job::JobState *get_states_array() const { return states_ptr_; }
  const int32_t *get_priorities_array() const { return priorities_ptr_; }
  size_t get_active_job_count() const { return current_count_; }

private:
  size_t get_or_create_job_index(const std::string &job_id);

  MmapBuffer buffer_;
  size_t max_jobs_;
  size_t current_count_ = 0;

  std::unordered_map<std::string, size_t> id_to_index_;
  mutable std::mutex job_mutex_;

  // SoA Pointers into mmap memory
  Job::JobState *states_ptr_ = nullptr;
  int32_t *priorities_ptr_ = nullptr;
  uint32_t *req_nodes_ptr_ = nullptr;
  uint64_t *req_mem_ptr_ = nullptr;

  std::vector<std::string> job_ids_;
};

} // namespace torque_ng