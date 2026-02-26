#include "allocation.hpp"
#include <sstream>
#include <algorithm>
#include <set>

/**
 * Returns a comma-separated string of the hardware IDs 
 * assigned to this job, suitable for cgroups.
 */
std::string allocation::get_cpuset_string() const {
    if (assigned_nodes.empty()) return "";

    // Use a set to sort IDs and remove duplicates (if multiple threads per core)
    std::set<int> core_ids;
    for (const auto* node : assigned_nodes) {
        core_ids.insert(node->id);
    }

    std::stringstream ss;
    for (auto it = core_ids.begin(); it != core_ids.end(); ++it) {
        if (it != core_ids.begin()) ss << ",";
        ss << *it;
    }
    return ss.str();
}

/**
 * Marks the resources in the global topology as free again.
 */
void allocation::release_resources() {
    for (auto* node : assigned_nodes) {
        node->available_threads++;
        // If the node has a parent (like a Socket or the Machine root), 
        // you would typically call aggregate_resources() to update the tree.
    }
    assigned_nodes.clear();
    memory_allocated = 0;
    gpus_allocated = 0;
}