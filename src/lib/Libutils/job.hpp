#pragma once
#include <string>

struct Job {
    uint32_t id;
    int required_cores;
    uint64_t required_memory;
    std::string user;
    int min_efficiency; // 1 for P-cores, 0 for E-cores
};