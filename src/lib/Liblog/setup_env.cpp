/*
 * torque-NG: Next Generation Resource Manager
 *
 * Copyright (c) 2026 Kenneth Nielson.
 * Portions Copyright (c) 1999-2000 Veridian Information Solutions, Inc.
 * Licensed under the OpenPBS v2.3 Software License.
 * SPDX-License-Identifier: OpenPBS-2.3
 */

#include "setup_env.h"
#include <fstream>
#include <iostream>
#include <cstdlib>

std::vector<std::string> setup_env(const std::filesystem::path& path) {
    std::vector<std::string> env_vars;
    std::ifstream efile(path);

    if (!efile.is_open()) {
        std::cerr << "torque-ng [ERROR]: Could not open env file: " << path << std::endl;
        return env_vars;
    }

    std::string line;
    while (std::getline(efile, line)) {
        // Skip empty lines or comments
        if (line.empty() || line[0] == '#') continue;

        // Check if the line is an environment variable name only
        if (line.find('=') == std::string::npos) {
            const char* pval = std::getenv(line.c_str());
            if (pval != nullptr) {
                env_vars.push_back(line + "=" + pval);
            }
        } else {
            env_vars.push_back(line);
        }
    }

    // Capture local environment overrides like PBSDEBUG
    const char* debug_val = std::getenv("PBSDEBUG");
    if (debug_val != nullptr) {
        env_vars.push_back(std::string("PBSDEBUG=") + debug_val);
    }

    return env_vars;
}