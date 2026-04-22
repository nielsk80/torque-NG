/*
 * torque-NG: Next Generation Resource Manager
 *
 * Copyright (c) 2026 Kenneth Nielson.
 * Portions Copyright (c) 1999-2000 Veridian Information Solutions, Inc.
 * Licensed under the OpenPBS v2.3 Software License.
 */

#include "TorqueErrors.hpp"
#include <string>
#include "log.h"
#include "safe_log.hpp"

namespace torque_ng {

void log_err(int err_code, std::string_view routine, std::string_view text) {
    // Convert the integer error to its descriptive text
    auto error_msg = Torque::get_error_description(static_cast<Torque::ErrorCode>(err_code));
    
    // Construct a formatted message
    std::string full_msg = std::string(routine) + ": " + std::string(text) + 
                           " (Error: " + std::string(error_msg) + ")";
    
    // Forward to the primary log_event function
    log_event(Torque::EventType::Error, Torque::EventClass::Server, "System", full_msg);
}

void log_event(Torque::EventType type, Torque::EventClass cls,
               std::string_view name, std::string_view text) {
  SafeLog::getInstance().record(type, cls, name, text);
}

} // namespace torque_ng