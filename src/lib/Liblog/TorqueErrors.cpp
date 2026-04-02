/*
 * torque-NG: Next Generation Resource Manager
 *
 * Copyright (c) 2026 Kenneth Nielson.
 * Portions Copyright (c) 1999-2000 Veridian Information Solutions, Inc.
 * All rights reserved.
 *
 * Licensed under the OpenPBS v2.3 Software License.
 * See the LICENSE file in the project root for full license details.
 *
 * SPDX-License-Identifier: OpenPBS-2.3
 */

#include "TorqueErrors.hpp"
#include <unordered_map>

namespace Torque {

std::string_view to_string(ErrorCode ec) {
  // The "Database" remains local to this translation unit
  static const std::unordered_map<ErrorCode, std::string_view> error_db = {
      {ErrorCode::None, "no error"},
      {ErrorCode::UnknownJobId, "Unknown Job Identifier"},
      {ErrorCode::NoAttribute, "Undefined Attribute"},
      {ErrorCode::AttrReadOnly, "Attempt to set read only attribute"},
      {ErrorCode::InvalidValue, "Invalid attribute value"},
      {ErrorCode::Permission, "Permission denied"},
      {ErrorCode::Internal, "Internal error"},
      {ErrorCode::SystemError, "System error"},
      {ErrorCode::CgroupFail, "Could not create cgroups for this job"},
      {ErrorCode::RmUnknown, "Resource unknown"},
      {ErrorCode::RmBadParam, "Parameter could not be used"},
      {ErrorCode::RmNoConnect, "Could not connect to resource manager"}};

  auto it = error_db.find(ec);
  return (it != error_db.end()) ? it->second : "Unknown Torque Error Code";
}

} // namespace Torque