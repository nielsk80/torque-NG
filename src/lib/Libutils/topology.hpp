// topology.hpp
#pragma once
#include <vector>
#include <memory>
#include <string>
#include <optional>

enum class NodeType { Machine, Socket, NumaNode, Core, ProcessingUnit, Accelerator };

class HardwareNode {
public:
    HardwareNode(NodeType t, int id, HardwareNode* p = nullptr) : type(t), id(id), parent(p) {}

    // Core Properties
    NodeType type;
    int id;
    HardwareNode* parent;
    std::string cpuset;
    std::string nodeset;

    int efficiency_class = -1; // For Hybrid/ARM systems: Higher = Performance Cores. 
                               // -1 indicates "unknown" or "not applicable"

    // Resource Tracking (Replaces the many counters in numa_chip.cpp)
    uint64_t total_memory = 0;
    uint64_t available_memory = 0;
    int total_threads = 0;
    int available_threads = 0;
    bool is_coherent_memory = false; // True for Grace Hopper GPU memory
    std::string memory_type;         // "DDR5", "HBM3", "LPDDR5X"

    // The "Composite" part: Nodes can contain other Nodes
    std::vector<std::unique_ptr<HardwareNode>> children;

    // Modern C++17 placement method (The Interface for our Strategy)
    bool is_available() const { return available_threads > 0; }
    int count_descendants(NodeType target_type) const;
    void display(int depth = 0) const;
    void aggregate_resources(); // Sums up resources from the bottom up
    std::string get_cgroup_path() const;

    std::string type_to_string(NodeType type) const; 

};