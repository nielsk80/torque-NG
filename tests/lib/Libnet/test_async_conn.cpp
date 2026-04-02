/**
 * @file test_async_conn.cpp
 * @brief Unit test for Coroutine-based Protobuf networking.
 *
 * Copyright (c) 2026 Kenneth Nielson.
 * Licensed under the OpenPBS v2.3 Software License.
 */

#include "Libnet/AsyncConnection.hpp"
#include "proto/tm_messages.pb.h"
#include <boost/asio.hpp>
#include <chrono>
#include <gtest/gtest.h>

namespace asio = boost::asio;
using namespace torque_ng::net;
using namespace torque_ng::tm;

// Server coroutine remains largely the same, but we ensure references remain
// valid
asio::awaitable<void> run_server(asio::ip::tcp::acceptor &acceptor,
                                 std::string &received_id) {
  try {
    auto socket = co_await acceptor.async_accept(asio::use_awaitable);
    AsyncConnection conn(std::move(socket));

    TmRequest req;
    co_await conn.receive_message(req);

    if (req.has_spawn_task()) {
      received_id = req.spawn_task().job_id();
    }

    TmResponse resp;
    resp.set_success(true);
    co_await conn.send_message(resp);
  } catch (const std::exception &e) {
    GTEST_LOG_(ERROR) << "Server error: " << e.what();
  }
}

TEST(AsyncConnectionTest, SendReceiveProtobuf) {
  asio::io_context ctx;
  const std::string test_job_id = "123.spanish-fork";
  std::string received_id = "";

  // Open acceptor on localhost with an ephemeral port
  asio::ip::tcp::acceptor acceptor(ctx, {asio::ip::tcp::v4(), 0});
  auto port = acceptor.local_endpoint().port();

  // 1. Start Server Coroutine
  // We pass the acceptor and received_id by reference since they live for the
  // duration of the test
  asio::co_spawn(ctx, run_server(acceptor, received_id), asio::detached);

  // 2. Start Client Coroutine
  // FIX: Pass the lambda as a factory to co_spawn rather than calling it
  // immediately. This allows Asio to copy/move the lambda into the coroutine's
  // stable memory state.
  asio::co_spawn(
      ctx,
      [port, test_job_id, &received_id, &ctx]() -> asio::awaitable<void> {
        try {
          asio::ip::tcp::socket socket(ctx);
          asio::ip::tcp::endpoint endpoint(
              asio::ip::address::from_string("127.0.0.1"), port);

          co_await socket.async_connect(endpoint, asio::use_awaitable);

          AsyncConnection conn(std::move(socket));

          TmRequest req;
          req.mutable_spawn_task()->set_job_id(test_job_id);
          co_await conn.send_message(req);

          TmResponse resp;
          co_await conn.receive_message(resp);
          EXPECT_TRUE(resp.success());
        } catch (const std::exception &e) {
          GTEST_LOG_(ERROR) << "Client error: " << e.what();
        }
      },
      asio::detached);

  // Run the event loop. In Spanish Fork, we want to ensure
  // we give the coroutines enough time to complete the handshake.
  ctx.run_for(std::chrono::seconds(2));

  EXPECT_EQ(test_job_id, received_id);
}