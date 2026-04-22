/*
 * torque-NG: Next Generation Resource Manager
 *
 * Copyright (c) 2026 Kenneth Nielson.
 * Portions Copyright (c) 1999-2000 Veridian Information Solutions, Inc.
 * All rights reserved.
 *
 * Licensed under the OpenPBS v2.3 Software License.
 * See the LICENSE file in the project root for full license details.
 *
 * SPDX-License-Identifier: OpenPBS-2.3
 */

#pragma once

#include "TorqueErrors.hpp"
#include <boost/asio.hpp>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/use_awaitable.hpp>
#include <arpa/inet.h> // For ntohl/htonl
#include <cstdint>
#include <memory>
#include <vector>

namespace asio = boost::asio;

namespace torque_ng::Libnet {

/**
 * @class AsyncConnection
 * @brief Manages a single TCP connection using Boost.Asio coroutines.
 * * This class handles the low-level framing for torque-NG communication,
 * ensuring that all messages are length-prefixed and subject to
 * safety size constraints.
 */
class AsyncConnection {
public:
    /**
     * @brief Constructor takes ownership of a connected socket.
     */
    explicit AsyncConnection(asio::ip::tcp::socket socket) 
        : m_socket(std::move(socket)) {}

    /**
     * @brief Initiates an asynchronous receive operation.
     * * Performs a two-stage read:
     * 1. Reads a 4-byte network-byte-order length header.
     * 2. Validates the size (100MB limit) to prevent memory exhaustion.
     * 3. Reads the actual payload into the provided buffer.
     * * @param out_data Vector to be resized and filled with received bytes.
     * @return asio::awaitable<Torque::ErrorCode> Success or specific Network/Protocol error.
     */
    asio::awaitable<Torque::ErrorCode> recv_async(std::vector<uint8_t>& out_data) {
        try {
            uint32_t msg_len_nbo = 0;
            
            // Stage 1: Read the 4-byte length header
            co_await asio::async_read(m_socket, 
                                      asio::buffer(&msg_len_nbo, sizeof(uint32_t)), 
                                      asio::use_awaitable);

            uint32_t msg_len = ntohl(msg_len_nbo);

            // HPC Safety: 100MB sanity check to prevent bad_alloc from corrupt headers
            if (msg_len > 104857600) { 
                co_return Torque::ErrorCode::ProtocolError;
            }

            out_data.resize(msg_len);

            // Stage 2: Read the actual message body
            co_await asio::async_read(m_socket, 
                                      asio::buffer(out_data), 
                                      asio::use_awaitable);

            co_return Torque::ErrorCode::None;
        } catch (const std::system_error& /*e*/) {
            co_return Torque::ErrorCode::NetworkError;
        } catch (...) {
            co_return Torque::ErrorCode::Internal;
        }
    }

    /**
     * @brief Initiates an asynchronous send operation.
     * * Automatically prepends the 4-byte length header before the payload
     * to ensure the remote end can frame the message correctly.
     * * @param data The raw bytes to transmit.
     * @return asio::awaitable<Torque::ErrorCode> Success or NetworkError.
     */
    asio::awaitable<Torque::ErrorCode> send_async(const std::vector<uint8_t>& data) {
        try {
            uint32_t len_nbo = htonl(static_cast<uint32_t>(data.size()));
            
            // Create a scatter-gather buffer sequence to send header and data in one go
            std::array<asio::const_buffer, 2> buffers = {
                asio::buffer(&len_nbo, sizeof(len_nbo)),
                asio::buffer(data)
            };

            co_await asio::async_write(m_socket, buffers, asio::use_awaitable);
            
            co_return Torque::ErrorCode::None;
        } catch (const std::system_error& /*e*/) {
            co_return Torque::ErrorCode::NetworkError;
        }
    }

    /**
     * @brief Returns a reference to the underlying Asio socket.
     */
    asio::ip::tcp::socket& socket() { 
        return m_socket; 
    }

private:
    asio::ip::tcp::socket m_socket;
};

} // namespace torque_ng::Libnet