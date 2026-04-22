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

#include "NetworkAddress.hpp"
#include "Message.hpp"
#include <functional>
#include <memory>
#include <atomic>

namespace torque_ng::network {

class MessageListener {
public:
    using message_handler = std::function<void(Message&&, int client_fd)>;

    explicit MessageListener(const NetworkAddress& address);
    ~MessageListener();

    MessageListener(const MessageListener&) = delete;
    MessageListener& operator=(const MessageListener&) = delete;

    // Methods reverted to snake_case
    void run(const message_handler& handler);
    void stop();

private:
    void setup_socket(); // Internal helper in snake_case

    NetworkAddress address_;
    int listen_fd_{-1};
    std::atomic<bool> running_{false};
};

} // namespace torque_ng::network