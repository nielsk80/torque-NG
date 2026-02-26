#include "factory.hpp"
#include <hwloc.h>

// Translates hwloc C-types to our Modern C++ NodeType
NodeType TopologyFactory::map_hwloc_type(hwloc_obj_type_t type) {
    switch (type) {
        case HWLOC_OBJ_MACHINE:     return NodeType::Machine;
        case HWLOC_OBJ_PACKAGE:     return NodeType::Socket;
        case HWLOC_OBJ_NUMANODE:    return NodeType::NumaNode;
        case HWLOC_OBJ_CORE:        return NodeType::Core;
        case HWLOC_OBJ_PU:          return NodeType::ProcessingUnit;
        case HWLOC_OBJ_PCI_DEVICE:  
        case HWLOC_OBJ_OS_DEVICE:   return NodeType::Accelerator;
        default:                    return NodeType::Machine;
    }
}

void TopologyFactory::walk_topology(HardwareNode* parent, hwloc_obj_t hw_obj, hwloc_topology_t topo) {
    if (!hw_obj) return;

    NodeType our_type = map_hwloc_type(hw_obj->type);
    auto node = std::make_unique<HardwareNode>(our_type, hw_obj->logical_index, parent);

    // --- Capture Memory (Specifically from NUMA nodes) ---
    if (our_type == NodeType::NumaNode && hw_obj->attr->numanode.local_memory > 0) {
        node->total_memory = hw_obj->attr->numanode.local_memory;
        node->available_memory = node->total_memory;
    }

    // --- Capture Threads (From leaf Processing Units) ---
    if (our_type == NodeType::ProcessingUnit) {
        node->total_threads = 1;
        node->available_threads = 1;
    }

    // Capture Cpusets
    if (hw_obj->cpuset) {
        char* buf;
        hwloc_bitmap_asprintf(&buf, hw_obj->cpuset);
        node->cpuset = buf;
        free(buf);
    }

    // Hybrid Core Efficiency detection (Corrected 7-argument call)
    if (our_type == NodeType::Core && hw_obj->cpuset) {
        int kind_idx = hwloc_cpukinds_get_by_cpuset(topo, hw_obj->cpuset, 0);
        if (kind_idx >= 0) {
            int efficiency = 0;
            if (hwloc_cpukinds_get_info(topo, kind_idx, nullptr, &efficiency, nullptr, nullptr, 0) == 0) {
                node->efficiency_class = efficiency;
            }
        }
    }

    // Recurse: build the tree exactly as hwloc sees it
    for (unsigned i = 0; i < hw_obj->arity; i++) {
        walk_topology(node.get(), hw_obj->children[i], topo);
    }

   // Use memory_arity and follow the linked list of memory_first_child
    hwloc_obj_t mem_child = hw_obj->memory_first_child;
    while (mem_child) {
        walk_topology(node.get(), mem_child, topo);
        mem_child = mem_child->next_sibling;
    }

    parent->children.push_back(std::move(node));
}

std::unique_ptr<HardwareNode> TopologyFactory::build_system_tree() {
    HwlocManager hwloc; // RAII handles init/load/destroy
    
    hwloc_obj_t root_obj = hwloc_get_root_obj(hwloc.topology);
    NodeType root_type = map_hwloc_type(root_obj->type);
    
    auto root_node = std::make_unique<HardwareNode>(root_type, root_obj->logical_index);

    // Populate the tree recursively
    for (unsigned i = 0; i < root_obj->arity; i++) {
        walk_topology(root_node.get(), root_obj->children[i], hwloc.topology);
    }

    return root_node;
}