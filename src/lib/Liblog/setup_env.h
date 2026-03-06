
/*
 * torque-NG: Next Generation Resource Manager
 *
 * Copyright (c) 2026 Kenneth Nielson.
 * Portions Copyright (c) 1999-2000 Veridian Information Solutions, Inc.
 * Licensed under the OpenPBS v2.3 Software License.
 * SPDX-License-Identifier: OpenPBS-2.3
 */

#pragma once

#include <vector>
#include <string>
#include <filesystem>

/**
 * setup_env - Reads a file and prepares a vector of environment strings.
 * Replaces the legacy char **envp with a safe container.
 */
std::vector<std::string> setup_env(const std::filesystem::path& path);