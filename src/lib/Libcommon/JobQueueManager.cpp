#include "JobQueueManager.hpp"

namespace torque_ng {

JobQueueManager::JobQueueManager(const std::string& state_file, size_t max_queues)
    : max_queues_(max_queues),
      buffer_(state_file, max_queues * (
          sizeof(Queue::QueueType) + 
          sizeof(Queue::QueueState) + 
          sizeof(uint32_t) * 2 + 
          sizeof(int32_t))) {

    uint8_t* raw_base = static_cast<uint8_t*>(buffer_.get_ptr());
    size_t offset = 0;

    types_ptr_ = reinterpret_cast<Queue::QueueType*>(raw_base + offset);
    offset += max_queues_ * sizeof(Queue::QueueType);

    states_ptr_ = reinterpret_cast<Queue::QueueState*>(raw_base + offset);
    offset += max_queues_ * sizeof(Queue::QueueState);

    max_run_ptr_ = reinterpret_cast<uint32_t*>(raw_base + offset);
    offset += max_queues_ * sizeof(uint32_t);

    cur_run_ptr_ = reinterpret_cast<uint32_t*>(raw_base + offset);
    offset += max_queues_ * sizeof(uint32_t);

    priority_ptr_ = reinterpret_cast<int32_t*>(raw_base + offset);
}

size_t JobQueueManager::get_or_create_queue_index(const std::string& name) {
    std::lock_guard<std::mutex> lock(queue_mutex_);
    auto it = name_to_index_.find(name);
    if (it != name_to_index_.end()) return it->second;

    size_t idx = current_count_++;
    name_to_index_[name] = idx;
    queue_names_.push_back(name);
    return idx;
}

void JobQueueManager::update_queue(const Queue& queue_update) {
    size_t idx = get_or_create_queue_index(queue_update.name());
    
    types_ptr_[idx]    = queue_update.type();
    states_ptr_[idx]   = queue_update.state();
    max_run_ptr_[idx]  = queue_update.max_running();
    priority_ptr_[idx] = queue_update.priority();

    buffer_.flush();
}

void JobQueueManager::increment_running_count(const std::string& name) {
    auto it = name_to_index_.find(name);
    if (it != name_to_index_.end()) {
        cur_run_ptr_[it->second]++; // Fast atomic-style increment in mmap
    }
}

void JobQueueManager::decrement_running_count(const std::string& name) {
    auto it = name_to_index_.find(name);
    if (it != name_to_index_.end() && cur_run_ptr_[it->second] > 0) {
        cur_run_ptr_[it->second]--;
    }
}

} // namespace torque_ng