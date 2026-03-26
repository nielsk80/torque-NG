/*
 * torque-NG: Next Generation Resource Manager
 *
 * Copyright (c) 2026 Kenneth Nielson.
 * Portions Copyright (c) 1999-2000 Veridian Information Solutions, Inc.
 * Licensed under the OpenPBS v2.3 Software License.
 */

#pragma once

#include "torque_ng.pb.h"
#include <string>
#include <algorithm>
#include <cctype>

namespace torque_ng {
namespace utils {

/**
 * @class JobStateMapper
 * @brief Utility to map between legacy Torque string/char states and Protobuf Enums.
 */
class JobStateMapper {
public:
    /**
     * Converts a string (e.g., "R", "RUNNING", "Q") to a torque_ng::Job::JobState enum.
     * @param state_str The input state string.
     * @return The corresponding JobState enum value. Defaults to TRANSIT if unknown.
     */
    static Job::JobState from_string(const std::string& state_str) {
        if (state_str.empty()) {
            return Job::TRANSIT;
        }

        // Create an uppercase version for case-insensitive matching
        std::string s = state_str;
        std::transform(s.begin(), s.end(), s.begin(),
                       [](unsigned char c){ return std::toupper(c); });

        // Handle single-character Torque codes (Legacy compat)
        if (s.length() == 1) {
            switch (s[0]) {
                case 'Q': return Job::QUEUED;
                case 'R': return Job::RUNNING;
                case 'H': return Job::HELD;
                case 'W': return Job::WAITING;
                case 'E': return Job::EXITING;
                case 'C': return Job::COMPLETE;
                case 'T': return Job::TRANSIT;
                default:  return Job::TRANSIT;
            }
        }

        // Handle full string names
        if (s == "QUEUED")   return Job::QUEUED;
        if (s == "RUNNING")  return Job::RUNNING;
        if (s == "HELD")     return Job::HELD;
        if (s == "WAITING")  return Job::WAITING;
        if (s == "EXITING")  return Job::EXITING;
        if (s == "COMPLETE") return Job::COMPLETE;
        if (s == "TRANSIT")  return Job::TRANSIT;

        return Job::TRANSIT;
    }

    /**
     * Helper to safely cast integers to JobSubState enums.
     */
    static Job::JobSubState sub_from_int(int32_t sub_state) {
        // Protobuf enums are compatible with int32_t
        return static_cast<Job::JobSubState>(sub_state);
    }

    // Inside JobStateMapper class
   static Job::JobSubState sub_from_string(const std::string& sub_str) {
      if (sub_str.empty()) return Job::SUBSTATE_NONE;
    
      // Add logic here to map your substate strings
      if (sub_str == "STAGE_IN") return Job::SUBSTATE_STAGEIN;
      // ...
    
      return Job::SUBSTATE_NONE;
   }
};

} // namespace utils
} // namespace torque_ng