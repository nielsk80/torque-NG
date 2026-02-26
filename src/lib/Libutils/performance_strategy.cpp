#include "strategy.hpp"

class PerformanceFirstStrategy : public PlacementStrategy {
public:
    std::vector<HardwareNode*> find_placement(HardwareNode* root, const Job& job) override {
        std::vector<HardwareNode*> selection;
        
        // 1. Recursive search for nodes matching efficiency and availability
        search_node(root, job, selection);

        return selection;
    }

private:
    void search_node(HardwareNode* node, const Job& job, std::vector<HardwareNode*>& selection) {
        if (!node) return;

        // Base Case: If this is a Core and matches efficiency
        if (node->type == NodeType::Core && node->efficiency_class >= job.min_efficiency) {
            if (node->available_threads > 0) {
                selection.push_back(node);
            }
        }

        // Recursive step
        for (auto& child : node->children) {
            search_node(child.get(), job, selection);
        }
    }
};