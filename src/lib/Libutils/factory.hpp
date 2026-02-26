#pragma once

#include <hwloc.h>
#include <memory>
#include <stdexcept>
#include "topology.hpp"

// RAII Wrapper for hwloc topology to prevent memory leaks
struct HwlocManager {
    hwloc_topology_t topology;
    HwlocManager() {
        if (hwloc_topology_init(&topology) < 0)
            throw std::runtime_error("Failed to initialize hwloc topology");
        if (hwloc_topology_load(topology) < 0) {
            hwloc_topology_destroy(topology);
            throw std::runtime_error("Failed to load hwloc topology");
        }
    }
    ~HwlocManager() {
        hwloc_topology_destroy(topology);
    }
};

class TopologyFactory {
public:
    static std::unique_ptr<HardwareNode> build_system_tree();

private:
    // Recursive helper that now replaces all style-specific logic
    static void walk_topology(HardwareNode* parent, hwloc_obj_t hw_obj, hwloc_topology_t topo);
    
    // Helper to bridge C-library types to our C++ enum
    static NodeType map_hwloc_type(hwloc_obj_type_t type);
};