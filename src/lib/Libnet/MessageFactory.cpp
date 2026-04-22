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

#include "MessageFactory.hpp"
#include "message_codec.hpp"
#include <unistd.h>
#include <vector>
#include <format>
#include <iostream>

namespace torque_ng::network {

std::optional<Message> MessageFactory::create_from_socket(int fd) {
    std::array<std::byte, codec::HEADER_SIZE> header_buffer{};
    
    // 1. Read the header
    ssize_t bytes_read = ::read(fd, header_buffer.data(), codec::HEADER_SIZE);
    if (bytes_read != static_cast<ssize_t>(codec::HEADER_SIZE)) {
        return std::nullopt; 
    }

    // 2. Decode and validate
    auto decoded = codec::decode_header(header_buffer);
    if (!decoded) {
        // Log error: Invalid Magic Number
        return std::nullopt;
    }

    // 3. Read the payload
    std::vector<std::byte> payload(decoded->payload_size);
    size_t total_payload_read = 0;
    
    while (total_payload_read < decoded->payload_size) {
        ssize_t n = ::read(fd, payload.data() + total_payload_read, 
                         decoded->payload_size - total_payload_read);
        if (n <= 0) return std::nullopt; // Connection closed or error
        total_payload_read += n;
    }

    return Message(decoded->type, std::move(payload));
}

} // namespace torque_ng::network