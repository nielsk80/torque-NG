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

class JobQueueManager {
public:
  // We pre-allocate space for a fixed number of queues (usually small, e.g.,
  // 500)
  JobQueueManager(const std::string &state_file, size_t max_queues = 512);

  // Updates or creates a queue definition
  void update_queue(const Queue &queue_update);

  // High-speed counters for the scheduler
  void increment_running_count(const std::string &name);
  void decrement_running_count(const std::string &name);

  // Direct access for the scheduler to check limits (e.g., max_run)
  const uint32_t *get_max_run_array() const { return max_run_ptr_; }
  const uint32_t *get_current_run_array() const { return cur_run_ptr_; }

private:
  size_t get_or_create_queue_index(const std::string &name);

  MmapBuffer buffer_;
  size_t max_queues_;
  size_t current_count_ = 0;

  std::unordered_map<std::string, size_t> name_to_index_;
  mutable std::mutex queue_mutex_;

  // SoA Pointers into mmap memory
  Queue::QueueType *types_ptr_ = nullptr;
  Queue::QueueState *states_ptr_ = nullptr;
  uint32_t *max_run_ptr_ = nullptr; // From qu_qs.qu_maxrun
  uint32_t *cur_run_ptr_ = nullptr; // Tracked live
  int32_t *priority_ptr_ = nullptr;

  std::vector<std::string> queue_names_;
};

} // namespace torque_ng