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

#include "LogTypes.hpp"
#include <fstream>
#include <mutex>
#include <string_view>
#include <filesystem>



class SafeLog {
public:
    static SafeLog& getInstance() {
        static SafeLog instance; // Guaranteed to be thread-safe in C++11 and later
        return instance;
    }
    // Rule of Five: Delete copy/assignment
    SafeLog(const SafeLog&) = delete;
    SafeLog& operator=(const SafeLog&) = delete;
    SafeLog(SafeLog&&) = delete;
    SafeLog& operator=(SafeLog&&) = delete;

    void record(Torque::EventType type, Torque::EventClass cls,
                std::string_view name, std::string_view text);
    void set_filter(uint32_t event_mask) {
        std::scoped_lock lock(m_mutex);
        m_event_mask = event_mask;
    }
    bool open(const std::filesystem::path& path); 
    void close(); 
    long get_size_kb();
    void set_hostname(std::string_view hostname) {
        std::scoped_lock lock(m_mutex);
        m_hostname = hostname;
    }

private:
    SafeLog(){}; // Private constructor for singleton pattern
    std::string get_current_date_string();
    void check_rotation_triggers();
    bool rotate();

    std::filesystem::path m_logpath; // Store the log path for size checks
    std::ofstream m_logstream; // Modern C++ File Handle
    std::mutex m_mutex;
    std::string m_hostname = "unknown";  // Cashed from pbs_log.cpp logic.
    uint32_t m_event_mask = 0xFFFF; // Replaces log_event_mask from pbs_log.cpp
    std::string m_last_rotation_date; // Format: YYMMDD, used for daily rotation checks
    std::size_t m_max_size_kb = 102400; // Default max size for rotation (100 MB)
};


