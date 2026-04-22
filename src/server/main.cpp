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

/**
 * @file main.cpp
 * @brief Entry point for the torque-NG server daemon.
 */

#include "ServerListener.hpp"
#include "log.h"
#include <boost/asio.hpp>
#include <iostream>
#include <exception>

/**
 * @brief Main entry point for pbs_server_ng.
 */
int main(int argc, char* argv[]) {
    try {
        // 1. Initialize the Asio IO Context (The engine)
        boost::asio::io_context io_context;

        // 2. Define the port (In production, this would come from a config file)
        uint16_t server_port = 15001;

        // 3. Initialize the Server Listener
        torque_ng::server::ServerListener listener(io_context, server_port);

        // 4. Start the listener coroutine. 
        // Because initial_suspend() returns suspend_never, this begins immediately.
        auto listen_task = listener.run();

        // 5. Start the event loop.
        // This call blocks until the io_context is stopped or out of work.
        torque_ng::log_event(
            Torque::EventType::System,
            Torque::EventClass::Server,
            "Main",
            "Starting torque-NG event loop...");

        io_context.run();

    } catch (const std::exception& e) {
        // Log critical startup failures
        std::cerr << "Critical Failure during startup: " << e.what() << std::endl;
        
        /* torque_ng::log_err(
            static_cast<int>(Torque::ErrorCode::Internal),
            "main",
            std::string("Daemon terminated unexpectedly: ") + e.what());
        */

        return 1;
    }

    return 0;
}