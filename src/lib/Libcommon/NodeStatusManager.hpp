/*
 * torque-NG: Next Generation Resource Manager
 *
 * Copyright (c) 2026 Kenneth Nielson.
 * Licensed under the OpenPBS v2.3 Software License.
 */

#pragma once

#include "MmapBuffer.hpp"
#include "torque_ng.pb.h"
#include <unordered_map>
#include <string>
#include <vector>
#include <mutex>

namespace torque_ng {

class NodeStatusManager {
public:
    // We pre-allocate space for a maximum number of nodes to avoid frequent remapping
    NodeStatusManager(const std::string& state_file, size_t max_nodes = 10000);

    // Updates a node's status in the mmap'd SoA. 
    // If the node is new, it registers a new index.
    void update_node(const Node& node_update);

    // Returns a snapshot of the current state for Protobuf responses (qstat -n)
    NodeStatusBatch get_batch_snapshot() const;

    // Direct access for the scheduler (The "Hot" path)
    const int32_t* get_pcpus_array() const { return pcpus_ptr_; }
    const uint64_t* get_mem_array() const { return mem_ptr_; }
    size_t get_node_count() const { return current_count_; }

private:
    size_t get_or_create_index(const std::string& name);

    MmapBuffer buffer_;
    size_t max_nodes_;
    size_t current_count_ = 0;

    // Mapping names to array indices
    std::unordered_map<std::string, size_t> name_to_index_;
    mutable std::mutex registration_mutex_;

    // Raw pointers into the MmapBuffer
    // These point to the "columns" in our Structure of Arrays
    torque_ng::Node::NodeState* states_ptr_ = nullptr;
    int32_t* pcpus_ptr_ = nullptr;
    uint64_t* mem_ptr_ = nullptr;
    
    // Note: Strings (names) are handled differently since they aren't fixed-width.
    // For this version, we store names in a standard vector and only mmap the numeric data.
    std::vector<std::string> node_names_;
};

} // namespace torque_ng
