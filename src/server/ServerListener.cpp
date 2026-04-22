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

#include "ServerListener.hpp"
#include "log.h"
#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>

namespace torque_ng::server {

/**
 * @brief Initialize the TCP acceptor on the specified port.
 */
ServerListener::ServerListener(boost::asio::io_context& io_context, unsigned short port)
    : m_acceptor(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)) {}

/**
 * @brief The acceptor loop.
 * uses asio::use_awaitable to suspend until a client connects.
 */
boost::asio::awaitable<void> ServerListener::run() {
    while (true) {
        // Suspend until a new socket is accepted
        auto socket = co_await m_acceptor.async_accept(boost::asio::use_awaitable);
        
        // Wrap the socket in our modernized AsyncConnection
        auto connection = std::make_shared<Libnet::AsyncConnection>(std::move(socket));
        
        // Start handle_client as a concurrent coroutine on the same executor
        boost::asio::co_spawn(m_acceptor.get_executor(), 
                              handle_client(std::move(connection)), 
                              boost::asio::detached);
    }
}

/**
 * @brief Receives requests and manages communication errors.
 */
boost::asio::awaitable<void> ServerListener::handle_client(
    std::shared_ptr<Libnet::AsyncConnection> connection) {
    
    std::vector<uint8_t> request_buffer;

    // Both recv_async and this function return asio::awaitable, so co_await works natively.
    Torque::ErrorCode ec = co_await connection->recv_async(request_buffer);

    if (ec != Torque::ErrorCode::None) {
        // Log using the modernized log_err function that bridges to our SafeLog singleton.
        torque_ng::log_err(static_cast<int>(ec), "ServerListener::handle_client", "Protocol communication failure");
        co_return;
    }

    // Logic for parsing request_buffer and executing server commands goes here
    
    co_return;
}

} // namespace torque_ng::server