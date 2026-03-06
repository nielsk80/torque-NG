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

#pragma once

#include <cstdint>
#include <sys/stat.h> // Required for permission bit definitions

namespace Torque {

/**
 * EventType - Replaces the PBSEVENT_ macros from log.h.
 * These are bitmask values used for filtering.
 */
enum class EventType : uint32_t {
    Error        = 0x0001,
    System       = 0x0002,
    Admin        = 0x0004,
    Job          = 0x0008,
    Usage        = 0x0010,
    Security     = 0x0020,
    Sched        = 0x0040,
    Debug        = 0x0080,
    Debug2       = 0x0100,
    ClientAuth   = 0x0200,
    Syslog       = 0x0400,
    Force        = 0x8000
};

/**
 * EventClass - Replaces PBS_EVENTCLASS_ macros from log.h.
 * Identifies the subsystem generating the log.
 */
enum class EventClass : uint16_t {
    Server   = 1,
    Queue    = 2,
    Job      = 3,
    Request  = 4,
    File     = 5,
    Acct     = 6,
    Node     = 7,
    Track    = 8
};

/**
 * LogSecurity - Modern wrapper for POSIX permission bits.
 * Useful for validate_path_security calls.
 */
enum class LogSecurity : int {
    Strict  = S_IWOTH | S_IWGRP, // Disallow world and group write
    Default = S_IWOTH            // Disallow world write
};

} // namespace Torque