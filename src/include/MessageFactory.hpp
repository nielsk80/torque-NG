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
#include <memory>
#include <optional>
#include <expected> // C++23, but we can use std::optional or a result type for C++20

namespace torque_ng::network {

/**
 * @class MessageFactory
 * @brief Responsible for reconstructing Message objects from raw descriptors.
 */
class MessageFactory {
public:
    /**
     * @brief Reads from a file descriptor and constructs a Message.
     * @param fd The socket file descriptor.
     * @return A unique_ptr to a Message, or nullopt if a protocol error occurs.
     */
    static std::optional<Message> create_from_socket(int fd);
};

} // namespace torque_ng::network