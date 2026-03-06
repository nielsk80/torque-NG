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

#include "pbs_log.h"
#include "safe_log.hpp"
#include "LogTypes.hpp"
#include <iostream>
#include <system_error>
#include <filesystem>

/**
 * log_record - The primary bridge. 
 * Maps legacy int types to the new Torque::EventType and EventClass enums.
 */
void log_record(int eventtype, int objclass, std::string_view objname, std::string_view text) {
    auto type = static_cast<Torque::EventType>(eventtype);
    auto cls  = static_cast<Torque::EventClass>(objclass);

    SafeLog::getInstance().record(type, cls, objname, text);
}

/**
 * log_open - Modernized to return std::error_code instead of -1.
 * This provides the daemon with specific OS-level error details.
 */
std::error_code log_open(std::string_view filename, std::string_view directory) {
    std::filesystem::path full_path = std::filesystem::path(directory) / filename;
    
    if (SafeLog::getInstance().open(full_path)) {
        return {}; // Success (default error_code)
    }
    
    // If open fails, we return a generic 'access denied' or 'io_error' 
    // depending on the security check results stored in the singleton.
    return std::make_error_code(std::errc::permission_denied);
}

/**
 * log_size - Returns the size via the singleton.
 */
long log_size() {
    return SafeLog::getInstance().get_size_kb();
}

/**
 * log_close - Cleanly closes the singleton's stream.
 */
void log_close(int msg) {
    SafeLog::getInstance().close();
}

/**
 * get_c_envp - Implementation of the environment converter.
 * Converts a vector of strings to a null-terminated array of pointers.
 */
std::vector<char*> get_c_envp(std::vector<std::string>& env_vars) {
    std::vector<char*> c_envp;
    
    // 1. Reserve memory to prevent reallocations during the loop
    c_envp.reserve(env_vars.size() + 1);

    // 2. Map each string's internal buffer to a pointer
    for (auto& s : env_vars) {
        c_envp.push_back(s.data());
    }

    // 3. POSIX requirements: the environment array must end with a NULL
    c_envp.push_back(nullptr);

    return c_envp;
}