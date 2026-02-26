// Test the new topology scanning routines.

#include <iostream>
#include "machine.hpp"
#include "topology.hpp"
#include "strategy.hpp"

int main() {
    // 1. Initialize the Hardware Singleton
    auto& machine = Machine::get_instance();
    auto& topology = Machine::get_topology();

    std::cout << "--- Initial System State ---" << std::endl;
    std::cout << "Threads: " << topology.available_threads << " / " << topology.total_threads << std::endl;
    std::cout << "Memory:  " << (topology.available_memory / 1024 / 1024) << " MB" << std::endl;
    std::cout << "----------------------------" << std::endl;

    // 2. Set the strategy (Targeting P-Cores first)
    machine.set_strategy(std::make_unique<PerformanceFirstStrategy>());
}
