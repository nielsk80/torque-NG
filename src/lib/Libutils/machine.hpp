// machine.hpp
#pragma once
#include "topology.hpp"
#include "strategy.hpp"
#include "cgroup_manager.hpp"
#include <memory>

class Machine {
public:
    // This is the global access point for the hardware topology
    static Machine& get_instance() {
        static Machine instance;
        return instance;
    }
    HardwareNode& get_topology();

    // Prevent copying to maintain Singleton integrity
    Machine(const Machine&) = delete;
    Machine& operator=(const Machine&) = delete;
    bool has_sufficient_resources(int cores_requested, uint64_t mem_requested);

    void set_strategy(std::unique_ptr<PlacementStrategy> strategy);

    bool schedule_job(const Job& job);

private:
    Machine(); // Private constructor
    std::unique_ptr<HardwareNode> root_node;
    std::unique_ptr<PlacementStrategy> current_strategy;
    std::unique_ptr<ICgroupManager> cgroup_mgr;
};