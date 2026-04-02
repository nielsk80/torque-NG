/*
 * torque-NG: Next Generation Resource Manager
 *
 * Copyright (c) 2026 Kenneth Nielson.
 * Portions Copyright (c) 1999-2000 Veridian Information Solutions, Inc.
 * Licensed under the OpenPBS v2.3 Software License.
 * SPDX-License-Identifier: OpenPBS-2.3
 */

#include "pbs_messages.h"
#include <unordered_map>

namespace Torque {

std::string_view pbse_to_txt(int err_code) {
  // Define the error database using modern string_view
  static const std::unordered_map<int, std::string_view> error_db = {
      {0, "no error"},
      {15001, "Unknown Job Identifier"},
      {15002, "Undefined Attribute"},
      {15003, "Attempt to set read only attribute"},
      {15004, "Invalid attribute value"},
      {15020, "Permission denied"},
      {15021, "Unknown Queue"},
      {15033, "Job exceeds configured limits"},
      {15034, "Invalid argument"},
      // Add more error codes from your legacy pbs_error_db.h as needed
  };

  auto it = error_db.find(err_code);
  if (it != error_db.end()) {
    return it->second;
  }

  return "Unknown PBS Error Code";
}

} // namespace Torque