/*
 * torque-NG: Next Generation Resource Manager
 *
 * Copyright (c) 2026 Kenneth Nielson.
 * Licensed under the OpenPBS v2.3 Software License.
 */

#include "NodeStatusManager.hpp"
#include <cstring>

namespace torque_ng {

NodeStatusManager::NodeStatusManager(const std::string &state_file,
                                     size_t max_nodes)
    : max_nodes_(max_nodes),
      // Calculate total size: (State + CPU + Mem) * max_nodes
      buffer_(state_file, max_nodes * (sizeof(Node::NodeState) +
                                       sizeof(int32_t) + sizeof(uint64_t))) {

  // Set up the SoA pointers by offsetting into the mmap'd region
  uint8_t *raw_base = static_cast<uint8_t *>(buffer_.get_ptr());

  states_ptr_ = reinterpret_cast<Node::NodeState *>(raw_base);
  pcpus_ptr_ = reinterpret_cast<int32_t *>(
      raw_base + (max_nodes_ * sizeof(Node::NodeState)));
  mem_ptr_ = reinterpret_cast<uint64_t *>(
      raw_base + (max_nodes_ * (sizeof(Node::NodeState) + sizeof(int32_t))));
}

size_t NodeStatusManager::get_or_create_index(const std::string &name) {
  std::scoped_lock lock(registration_mutex_);

  auto it = name_to_index_.find(name);
  if (it != name_to_index_.end()) {
    return it->second;
  }

  if (current_count_ >= max_nodes_) {
    throw std::runtime_error("Node capacity reached. Resize required.");
  }

  size_t new_idx = current_count_++;
  name_to_index_[name] = new_idx;
  node_names_.push_back(name);
  return new_idx;
}

void NodeStatusManager::update_node(const Node &node_update) {
  size_t idx = get_or_create_index(node_update.name());

  // Direct memory writes - These are immediately "visible" to the OS for
  // persistence
  states_ptr_[idx] = node_update.state();
  pcpus_ptr_[idx] = node_update.pcpus();
  // Assuming available memory comes from resources in your model
  // mem_ptr_[idx] = ...;

  // Optional: flush to disk asynchronously
  buffer_.flush();
}

NodeStatusBatch NodeStatusManager::get_batch_snapshot() const {
  NodeStatusBatch batch;
  for (size_t i = 0; i < current_count_; ++i) {
    batch.add_names(node_names_[i]);
    batch.add_states(states_ptr_[i]);
    batch.add_pcpus(pcpus_ptr_[i]);
    // batch.add_available_mem(mem_ptr_[i]);
  }
  return batch;
}

} // namespace torque_ng
