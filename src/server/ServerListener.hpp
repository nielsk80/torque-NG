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

#include "AsyncConnection.hpp"
#include <boost/asio.hpp>
#include <boost/asio/awaitable.hpp>
#include <memory>
#include <vector>

/**
 * @namespace torque_ng::server
 * @brief contains components responsible for the pbs_server_ng daemon logic.
 */
namespace torque_ng::server {

/**
 * @class ServerListener
 * @brief Manages the high-level TCP listening socket and client coroutine spawning.
 * * This class encapsulates the boost::asio::ip::tcp::acceptor and provides
 * a coroutine-based loop for accepting new connections and delegating them
 * to individual client handlers.
 */
class ServerListener {
public:
    /**
     * @brief Constructs a new Server Listener object.
     * @param io_context The Boost.Asio io_context to run on.
     * @param port The TCP port to listen on for incoming connections.
     */
    explicit ServerListener(boost::asio::io_context& io_context, unsigned short port);

    /**
     * @brief The main execution loop of the listener.
     * * Continuously accepts incoming TCP connections. For every connection, 
     * it creates an AsyncConnection object and spawns a handle_client coroutine.
     * * @return boost::asio::awaitable<void> Coroutine handle.
     */
    boost::asio::awaitable<void> run();

private:
    /**
     * @brief Handles the lifecycle of a single client connection.
     * * Receives raw data from the connection and prepares it for processing.
     * Replaces the legacy NetTask return type to ensure compatibility with 
     * the Boost.Asio executor.
     * * @param connection Shared pointer to the established AsyncConnection.
     * @return boost::asio::awaitable<void> Coroutine handle.
     */
    boost::asio::awaitable<void> handle_client(std::shared_ptr<Libnet::AsyncConnection> connection);

    boost::asio::ip::tcp::acceptor m_acceptor; /**< The underlying Asio TCP acceptor. */
};

} // namespace torque_ng::server