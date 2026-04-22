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

#include "NetworkAddress.hpp"
#include <format>
#include <stdexcept>
#include <netinet/in.h> // for htons
#include <arpa/inet.h>  // for inet_pton
#include <cstring>      // for std::memset

namespace torque_ng::network {

/**
 * @brief Constructs a new NetworkAddress object.
 * * @param host The hostname or IP address string.
 * @param port The port number.
 * @param family The protocol family (Defaulting to IPv4).
 */
NetworkAddress::NetworkAddress(std::string_view host, uint16_t port, ProtocolFamily family)
    : m_hostname(host), m_port(port), m_family(family) {
    
    if (host.empty()) {
        throw std::invalid_argument("Hostname cannot be empty");
    }
}

/**
 * @brief Returns a string representation of the address.
 * * For IPv6, it follows the RFC 3986 format with square brackets.
 */
std::string NetworkAddress::toString() const {
    if (m_family == ProtocolFamily::IPv6) {
        return std::format("[{}]:{}", m_hostname, m_port);
    }
    return std::format("{}:{}", m_hostname, m_port);
}

struct sockaddr_in NetworkAddress::to_sockaddr_in() const {
    struct sockaddr_in addr{};
    std::memset(&addr, 0, sizeof(addr));
    
    addr.sin_family = AF_INET;
    addr.sin_port = htons(m_port);
    
    // Convert string hostname/IP to binary representation
    if (inet_pton(AF_INET, m_hostname.c_str(), &addr.sin_addr) <= 0) {
        // In a real HPC environment, you might want to handle hostname 
        // resolution (getaddrinfo) here if m_hostname isn't a raw IP.
    }
    
    return addr;
}

} // namespace torque_ng::network