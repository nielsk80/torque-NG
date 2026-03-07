/*
 * Copyright (C) 2026 Kenneth Nielson
 * Part of the torque-NG project.
 *
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE', which is part of this source code package.
 */

#pragma once

#include "pbs_config.h"

#ifdef __cplusplus
#include <string>
#include <string_view>
#include "safe_log.hpp" // Your new C++ logging engine

namespace torque_ng {

/**
 * @brief Modern Type-Safe Log Levels for torque-NG
 */
enum class LogLevel {
    ERROR = 0,
    WARNING = 1,
    INFO = 2,
    DEBUG = 3,
    TRACE = 4
};

// Modern C++ API: No more .c_str() required for these!
void log_event(LogLevel level, std::string_view facility, std::string_view message);
void log_err(int err_code, std::string_view routine, std::string_view text);

} // namespace torque_ng

extern "C" {
#endif

// --- Legacy Compatibility Layer ---
// Standard TORQUE event types found in legacy pbs_log.h
#define PBSEVENT_ERROR      0x0001
#define PBSEVENT_SYSTEM     0x0002
#define PBSEVENT_ADMIN      0x0004
#define PBSEVENT_JOB        0x0008
#define PBSEVENT_JOB_USAGE  0x0010
#define PBSEVENT_SECURITY   0x0020
#define PBSEVENT_SCHED      0x0040
#define PBSEVENT_DEBUG      0x0080
#define PBSEVENT_DEBUG2     0x0100

#define PBS_EVENTCLASS_SERVER 1
#define PBS_EVENTCLASS_MOM    2
#define PBS_EVENTCLASS_PROG   3

// Legacy C-style signatures
void log_event(int eventtype, int eventclass, const char* entity, const char* message);
void log_err(int err_code, const char* routine, const char* text);

} // extern "C"

