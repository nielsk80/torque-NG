/*
 * torque-NG: Next Generation Resource Manager
 *
 * Copyright (c) 2026 Kenneth Nielson.
 * All rights reserved.
 *
 * Licensed under the OpenPBS v2.3 Software License.
 * See the LICENSE file in the project root for full license details.
 *
 * SPDX-License-Identifier: OpenPBS-2.3
 */

#pragma once

#include <string>
#include <string_view>
#include <cstdint>
#include <netinet/in.h> // For sockaddr_in

namespace torque_ng::network {

/**
 * @brief Protocol families supported by torque-NG.
 */
enum class ProtocolFamily {
    IPv4,
    IPv6,
    Unspecified
};

/**
 * @brief Represents a validated network endpoint.
 * * Supports both IPv4 and IPv6 formatting using modern C++20 standards.
 */
class NetworkAddress {
public:
    /**
     * @brief Construct a NetworkAddress with a host and port.
     * @param host Hostname or IP address.
     * @param port Network port.
     * @param family Protocol family (defaults to IPv4).
     */
    NetworkAddress(std::string_view host, uint16_t port, ProtocolFamily family = ProtocolFamily::IPv4);

    [[nodiscard]] std::string toString() const;
    [[nodiscard]] ProtocolFamily family() const noexcept { return m_family; }
    [[nodiscard]] uint16_t port() const noexcept { return m_port; }
    [[nodiscard]] std::string_view hostname() const noexcept { return m_hostname; }
    /**
     * @brief Converts the NetworkAddress to a standard Linux sockaddr_in structure.
     * @return struct sockaddr_in for use with socket system calls.
     */
    [[nodiscard]] struct ::sockaddr_in to_sockaddr_in() const;

private:
    std::string m_hostname;
    uint16_t m_port;
    ProtocolFamily m_family;
};

} // namespace torque_ng::network