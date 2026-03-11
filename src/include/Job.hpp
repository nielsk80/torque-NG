/*
 * torque-NG: Next Generation Resource Manager
 *
 * Copyright (c) 2026 Kenneth Nielson.
 * Portions Copyright (c) 1999-2000 Veridian Information Solutions, Inc.
 * Licensed under the OpenPBS v2.3 Software License.
 */

#pragma once

#include "job_status.pb.h" // Generated Protobuf header
#include <string>
#include <string_view>
#include <map>
#include <memory>

namespace torque_ng {

class Job {
public:
    // Constructors
    explicit Job(std::string_view job_id);
    
    // Disable copying for performance; allow moving
    Job(const Job&) = delete;
    Job& operator=(const Job&) = delete;
    Job(Job&&) noexcept = default;
    Job& operator=(Job&&) noexcept = default;

    // Setters using C++17 string_view
    void set_owner(std::string_view owner);
    void set_state(int32_t state);
    void add_attribute(std::string_view key, std::string_view value);

    // Getters
    std::string_view get_id() const;
    int32_t          get_state() const;
    
    // Serialization for Libnet
    std::string serialize() const;

private:
    // This class encapsulates the Protobuf message
    // Making it a unique_ptr keeps the header light
    std::unique_ptr<JobStatusUpdate> m_data;
};

} // namespace torque_ng