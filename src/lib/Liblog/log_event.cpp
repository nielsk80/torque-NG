/*
 * torque-NG: Next Generation Resource Manager
 *
 * Copyright (c) 2026 Kenneth Nielson.
 * Portions Copyright (c) 1999-2000 Veridian Information Solutions, Inc.
 * Licensed under the OpenPBS v2.3 Software License.
 */

#include "log.h"
#include "safe_log.hpp"

namespace torque_ng {

void log_event(Torque::EventType type, Torque::EventClass cls,
               std::string_view name, std::string_view text) {
  SafeLog::getInstance().record(type, cls, name, text);
}

} // namespace torque_ng