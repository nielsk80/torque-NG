#pragma once
#include "JobStruct.hpp"
#include "topology.hpp"
#include <vector>

class PlacementStrategy {
public:
  virtual ~PlacementStrategy() = default;

  // Returns a list of nodes where the job should be placed
  virtual std::vector<HardwareNode *> find_placement(HardwareNode *root,
                                                     const Job &job) = 0;
};
