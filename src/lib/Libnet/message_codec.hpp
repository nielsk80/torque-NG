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

#include "Message.hpp"
#include <array>

namespace torque_ng::network::codec {

// TNG! in hex
inline constexpr uint32_t MAGIC_NUMBER = 0x544E4721;
inline constexpr uint16_t PROTOCOL_VERSION = 1;
inline constexpr size_t HEADER_SIZE = 12;

/**
 * @brief Encodes the header into a fixed-size buffer.
 * Format: Magic(4) | Version(2) | Type(2) | Size(4)
 */
std::array<std::byte, HEADER_SIZE> encode_header(MessageType type, uint32_t size);

/**
 * @brief Decodes a raw buffer into header components.
 */
struct DecodedHeader {
    uint16_t version;
    MessageType type;
    uint32_t payload_size;
};

std::optional<DecodedHeader> decode_header(std::span<const std::byte, HEADER_SIZE> buffer);

} // namespace torque_ng::network::codec