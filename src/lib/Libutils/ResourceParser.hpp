/*
 * torque-NG: Next Generation Resource Manager
 *
 * Copyright (c) 2026 Kenneth Nielson.
 * Portions Copyright (c) 1999-2000 Veridian Information Solutions, Inc.
 * Licensed under the OpenPBS v2.3 Software License.
 */

#pragma once
#include "torque_ng.pb.h" // Your generated Protobuf header
#include <map>
#include <string>
#include <string_view>

namespace torque_ng::utils {

class ResourceParser {
public:
  // Parses a string like "mem=2gb,nodes=1" into the Protobuf message
  static void parse_into(std::string_view raw_resources,
                         torque_ng::ResourceList *target);

  // Helper to convert strings like "2gb", "512mb" to raw bytes
  static uint64_t string_to_size(std::string_view size_str);

  static uint64_t string_to_duration(std::string_view dur_str);

private:
  static void handle_resource(std::string_view key, std::string_view value,
                              torque_ng::ResourceList *target);
};

} // namespace torque_ng::utils