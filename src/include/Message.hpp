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

#include <vector>
#include <cstdint>
#include <span>
#include <optional>

namespace torque_ng::network {

/**
 * @brief Identifiers for Protobuf-serialized payloads.
 */
enum class MessageType : uint16_t {
    Unknown = 0,
    JobSubmit = 1,
    JobStatus = 2,
    NodeUpdate = 3,
    Heartbeat = 4,
    ProtocolError = 999
};

/**
 * @class Message
 * @brief Encapsulates a serialized network packet with a type-safe header.
 */
class Message {
public:
    Message(MessageType type, std::vector<std::byte> payload);
    
    [[nodiscard]] MessageType type() const noexcept { return type_; }
    [[nodiscard]] std::span<const std::byte> payload() const noexcept { return payload_; }
    [[nodiscard]] size_t payload_size() const noexcept { return payload_.size(); }

private:
    MessageType type_;
    std::vector<std::byte> payload_;
};

} // namespace torque_ng::network