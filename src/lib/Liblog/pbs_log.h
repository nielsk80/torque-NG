/*
 * torque-NG: Next Generation Resource Manager
 *
 * Copyright (c) 2026 Kenneth Nielson.
 * Portions Copyright (c) 1999-2000 Veridian Information Solutions, Inc.
 * Licensed under the OpenPBS v2.3 Software License.
 * SPDX-License-Identifier: OpenPBS-2.3
 */

#pragma once

#include "LogTypes.hpp"
#include <string_view>
#include <string>
#include <vector>
#include <system_error>

/**
 * Legacy Bridge Functions
 * These allow existing TORQUE code to function while using the SafeLog engine.
 */

// Primary logging entry point
void log_record(int eventtype, int objclass, std::string_view objname, std::string_view text);

// Lifecycle management
std::error_code log_open(std::string_view filename, std::string_view directory);
void log_close(int msg);

// Hostname and IP resolution for daemon identification
void log_set_hostname_sharelogging(std::string_view server_name, std::string_view server_port);

// File metrics
long log_size();

/**
 * Modern Utility Functions
 */

/**
 * get_c_envp - Converts modern string vectors to C-style arrays for syscalls like execve.
 * Note: The returned pointers are valid only as long as env_vars is in scope and unchanged.
 */
std::vector<char*> get_c_envp(std::vector<std::string>& env_vars);
