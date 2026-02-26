// allocation.hpp
#pragma once

#include <vector>
#include <string>
#include <cstdint>
#include "topology.hpp"

class allocation {
public:
    allocation() = default;

    // The core of the refactor: tracking the specific hardware nodes assigned
    std::vector<HardwareNode*> assigned_nodes;

    uint64_t memory_allocated = 0;
    int      gpus_allocated = 0;

    /**
     * Returns true if the core requirement for the job has been met.
     */
    bool fully_placed(int cores_required) const {
        return assigned_nodes.size() >= static_cast<size_t>(cores_required);
    }

    /**
     * Aggregates the IDs of all assigned_nodes into a cgroup-compatible string.
     */
    std::string get_cpuset_string() const;

    /**
     * Returns threads/resources to the HardwareNodes in the global topology.
     */
    void release_resources();
};