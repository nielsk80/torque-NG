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

#include "message_codec.hpp"
#include <bit>

namespace torque_ng::network::codec {

std::array<std::byte, HEADER_SIZE> encode_header(MessageType type, uint32_t size) {
    std::array<std::byte, HEADER_SIZE> buffer{};
    
    // Manual packing for endian independence (Network Byte Order / Big Endian)
    auto pack32 = [&](uint32_t val, size_t offset) {
        buffer[offset]     = static_cast<std::byte>((val >> 24) & 0xFF);
        buffer[offset + 1] = static_cast<std::byte>((val >> 16) & 0xFF);
        buffer[offset + 2] = static_cast<std::byte>((val >> 8) & 0xFF);
        buffer[offset + 3] = static_cast<std::byte>(val & 0xFF);
    };

    auto pack16 = [&](uint16_t val, size_t offset) {
        buffer[offset]     = static_cast<std::byte>((val >> 8) & 0xFF);
        buffer[offset + 1] = static_cast<std::byte>(val & 0xFF);
    };

    pack32(MAGIC_NUMBER, 0);
    pack16(PROTOCOL_VERSION, 4);
    pack16(static_cast<uint16_t>(type), 6);
    pack32(size, 8);

    return buffer;
}

std::optional<DecodedHeader> decode_header(std::span<const std::byte, HEADER_SIZE> buffer) {
    auto unpack32 = [&](size_t offset) -> uint32_t {
        return (static_cast<uint32_t>(buffer[offset]) << 24) |
               (static_cast<uint32_t>(buffer[offset + 1]) << 16) |
               (static_cast<uint32_t>(buffer[offset + 2]) << 8) |
               static_cast<uint32_t>(buffer[offset + 3]);
    };

    if (unpack32(0) != MAGIC_NUMBER) return std::nullopt;

    uint16_t version = (static_cast<uint16_t>(buffer[4]) << 8) | static_cast<uint16_t>(buffer[5]);
    uint16_t type_raw = (static_cast<uint16_t>(buffer[6]) << 8) | static_cast<uint16_t>(buffer[7]);
    uint32_t p_size = unpack32(8);

    return DecodedHeader{version, static_cast<MessageType>(type_raw), p_size};
}

} // namespace torque_ng::network::codec