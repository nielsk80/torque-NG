// This is going to replace numa_socket.cpp, numa_chip.cpp, numa_core.cpp, and numa_thread.cpp.  
// It will be used to determine the topology of the system and how many sockets, 
// chips, cores, and threads are present.

#include "topology.hpp"
#include "pbs_log.h"
#include <sstream>
#include <string>

// Helper to count specific types of children (e.g., how many GPUs or Cores)
int HardwareNode::count_descendants(NodeType target_type) const {
    int count = 0;
    for (const auto& child : children) {
        if (child->type == target_type) count++;
        count += child->count_descendants(target_type);
    }
    return count;
}

// Aggregates resources from the bottom up
void HardwareNode::aggregate_resources() {
   // If we are a leaf node, keep our hardware values and stop recursing
   if (type == NodeType::ProcessingUnit || type == NodeType::NumaNode) {
        return; 
    }
    // Reset counters before aggregation
    total_threads = 0;
    available_threads = 0;
    total_memory = 0;
    available_memory = 0;

    if (children.empty()) {
        // This is a leaf node (like a Core or PU)
        // Values should already be set by the Factory
        return;
    }

    for (auto& child : children) {
        child->aggregate_resources();
        total_threads += child->total_threads;
        available_threads += child->available_threads;
        total_memory += child->total_memory;
        available_memory += child->available_memory;
    }
}

void HardwareNode::display(int depth) const {
    std::string indent(depth * 2, ' ');
    
    std::string msg = indent + "[Type " + std::to_string(static_cast<int>(type)) + "] " +
                      "ID: " + std::to_string(id) + 
                      " | Threads: " + std::to_string(available_threads) + "/" + std::to_string(total_threads);
    log_event(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, "Machine", msg.c_str());

    if (efficiency_class != -1) {
        std::string msg = " | Efficiency: " + std::to_string(efficiency_class);
        log_event(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, "Machine", msg.c_str());
    }

    if (!cpuset.empty()) {
        std::string msg = " | CPUSet: " + cpuset;
        log_event(PBSEVENT_SYSTEM, PBS_EVENTCLASS_SERVER, "Machine", msg.c_str());
    }

    for (const auto& child : children) {
        child->display(depth + 1);
    }
}

std::string HardwareNode::get_cgroup_path() const {
    if (parent == nullptr) {
        return ""; // This is a recursive routine. Do not assume /sys/fs/cgroup.
    }
        
    // Recursively build the path: /package_0/core_1/pu_0
    std::string path = parent->get_cgroup_path();
    return path + "/" + type_to_string(this->type) + "_" + std::to_string(this->id);
}

std::string HardwareNode::type_to_string(NodeType type) const {
    switch (type) {
        case NodeType::Machine:        return "machine";
        case NodeType::Socket:         return "socket";
        case NodeType::NumaNode:       return "numa";
        case NodeType::Core:           return "core";
        case NodeType::ProcessingUnit: return "pu";
        case NodeType::Accelerator:    return "accel";
        default:                       return "hw";
    }
}