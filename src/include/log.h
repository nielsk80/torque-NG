/*
 * torque-NG: Next Generation Resource Manager
 *
 * Copyright (c) 2026 Kenneth Nielson.
 * All rights reserved.
 *
 * This file replaces the legacy TORQUE log.h with a modern C++17 interface.
 */

#pragma once

#include "LogTypes.hpp"
#include <string_view>

namespace torque_ng {

/**
 * @brief Primary logging interface for torque-NG components.
 * * Uses std::string_view to avoid unnecessary allocations and utilizes
 * the underlying SafeLog singleton.
 *
 * @param type The EventType (Error, System, Job, etc.) defined in LogTypes.hpp
 * @param cls  The EventClass (Server, MOM, etc.) defined in LogTypes.hpp
 * @param name The name of the object or facility being logged
 * @param text The log message content
 */
void log_event(
    Torque::EventType type, 
    Torque::EventClass cls, 
    std::string_view name, 
    std::string_view text);

/**
 * @brief Error-specific logging helper.
 * * Maps PBS error codes to human-readable text using the Liblog database.
 */
void log_err(
    int err_code, 
    std::string_view routine, 
    std::string_view text);

} // namespace torque_ng