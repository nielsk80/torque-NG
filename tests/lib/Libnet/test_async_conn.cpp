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

#include "AsyncConnection.hpp"
#include "TorqueErrors.hpp"
#include <boost/asio.hpp>
#include <gtest/gtest.h>
#include <vector>
#include <string>
#include <memory>

// Use the modernized namespaces defined in the Libnet refactor [cite: 921, 966]
using namespace torque_ng::Libnet;
namespace asio = boost::asio;
using asio::ip::tcp;

/**
 * @brief Helper to simulate a server that echoes back data.
 * Utilizes the new RecvAwaiter and SendAwaiter patterns.
 */
asio::awaitable<void> run_echo_server(tcp::acceptor& acceptor) {
    auto socket = co_await acceptor.async_accept(asio::use_awaitable);
    AsyncConnection conn(std::move(socket)); // [cite: 925]

    std::vector<uint8_t> buffer;
    
    // Stage 1: Receive raw bytes using the new coroutine awaiter.
    // This includes the 100MB safety check internally[cite: 934, 940].
    Torque::ErrorCode recv_ec = co_await conn.recv_async(buffer); // [cite: 964]
    
    if (recv_ec == Torque::ErrorCode::None) {
        // Stage 2: Echo the same bytes back to the client[cite: 950].
        co_await conn.send_async(buffer);
    }
}

/**
 * @class AsyncConnectionTest
 * @brief Test fixture for Network-level async connection testing.
 */
class AsyncConnectionTest : public ::testing::Test {
protected:
    asio::io_context io_ctx;
};

/**
 * @brief Test: Verify basic data integrity through the echo server.
 */
TEST_F(AsyncConnectionTest, BasicSendAndReceive) {
    tcp::acceptor acceptor(io_ctx, tcp::endpoint(tcp::v4(), 0));
    unsigned short port = acceptor.local_endpoint().port();

    std::string test_data = "torque-NG-test-payload";
    std::vector<uint8_t> send_buf(test_data.begin(), test_data.end());
    std::vector<uint8_t> recv_buf;

    // Launch server coroutine
    asio::co_spawn(io_ctx, run_echo_server(acceptor), asio::detached);

    // Run client logic in another coroutine
    asio::co_spawn(io_ctx, [&]() -> asio::awaitable<void> {
        tcp::socket socket(io_ctx);
        co_await socket.async_connect(
            tcp::endpoint(asio::ip::make_address("127.0.0.1"), port), 
            asio::use_awaitable);

        AsyncConnection conn(std::move(socket)); // [cite: 925]

        // Test sending via the new SendAwaiter [cite: 950]
        Torque::ErrorCode send_ec = co_await conn.send_async(send_buf);
        EXPECT_EQ(send_ec, Torque::ErrorCode::None);

        // Test receiving via the new RecvAwaiter [cite: 964]
        Torque::ErrorCode recv_ec = co_await conn.recv_async(recv_buf);
        EXPECT_EQ(recv_ec, Torque::ErrorCode::None);

        // Verify data integrity
        std::string result_data(recv_buf.begin(), recv_buf.end());
        EXPECT_EQ(test_data, result_data);

        co_return;
    }, asio::detached);

    io_ctx.run();
}

/**
 * @brief Test: Ensure the 100MB safety limit is enforced.
 * This verifies the protection against bad_alloc from corrupt headers[cite: 940].
 */
TEST_F(AsyncConnectionTest, RejectsExcessiveMessageSize) {
    tcp::acceptor acceptor(io_ctx, tcp::endpoint(tcp::v4(), 0));
    unsigned short port = acceptor.local_endpoint().port();

    asio::co_spawn(io_ctx, [&]() -> asio::awaitable<void> {
        auto socket = co_await acceptor.async_accept(asio::use_awaitable);
        AsyncConnection conn(std::move(socket));
        
        std::vector<uint8_t> data;
        Torque::ErrorCode ec = co_await conn.recv_async(data); // [cite: 964]
        
        // Expect ProtocolError because we will send a fake header > 100MB [cite: 940]
        EXPECT_EQ(ec, Torque::ErrorCode::ProtocolError); 
    }, asio::detached);

    asio::co_spawn(io_ctx, [&]() -> asio::awaitable<void> {
        tcp::socket socket(io_ctx);
        co_await socket.async_connect(
            tcp::endpoint(asio::ip::make_address("127.0.0.1"), port), 
            asio::use_awaitable);

        // Send a bad header: 200MB (in Network Byte Order) [cite: 940]
        uint32_t bad_len = htonl(200 * 1024 * 1024);
        co_await asio::async_write(socket, asio::buffer(&bad_len, 4), asio::use_awaitable);
    }, asio::detached);

    io_ctx.run();
}