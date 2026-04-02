/*
 * torque-NG: Next Generation Resource Manager
 *
 * Copyright (c) 2026 Kenneth Nielson.
 * Portions Copyright (c) 1999-2000 Veridian Information Solutions, Inc.
 * Licensed under the OpenPBS v2.3 Software License.
 */

#include "Job.hpp"
#include <iomanip>
#include <iostream>

int main() {
  std::cout << "Initializing torque-NG Libproto vertical slice test..."
            << std::endl;

  // Create job instance
  torque_ng::Job test_job("9876.spanish-fork-cluster");

  // Populate data
  test_job.set_owner("knielson");
  test_job.set_state(1); // Running state
  test_job.set_resources(1024 * 1024 * 256, 1024 * 1024 * 512, 120.0);
  test_job.add_attribute("queue", "production");

  // Serialize to binary string
  std::string payload = test_job.serialize();

  if (payload.empty()) {
    std::cerr << "Error: Serialization produced empty payload." << std::endl;
    return 1;
  }

  std::cout << "Successfully serialized Job: " << test_job.get_id()
            << std::endl;
  std::cout << "Payload size: " << payload.size() << " bytes" << std::endl;

  // Hex preview to verify binary structure
  std::cout << "Binary Hex Preview: ";
  for (size_t i = 0; i < std::min(payload.size(), (size_t)12); ++i) {
    std::cout << std::hex << std::setw(2) << std::setfill('0')
              << (static_cast<int>(static_cast<unsigned char>(payload[i])))
              << " ";
  }
  std::cout << std::dec << "..." << std::endl;

  // 7. Test Deserialization (The Server side)
  std::cout << "Testing Deserialization (Round-trip)..." << std::endl;
  auto server_job = torque_ng::Job::deserialize(payload);

  if (server_job && server_job->get_id() == "9876.spanish-fork-cluster") {
    std::cout << "Round-trip successful! Job ID matched: "
              << server_job->get_id() << std::endl;
  } else {
    std::cerr << "Round-trip failed!" << std::endl;
    return 1;
  }

  return 0;
}