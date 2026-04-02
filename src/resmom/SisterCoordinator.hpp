/**
 * @file SisterCoordinator.hpp
 * @brief Manages parallel communication with sister nodes for multi-node jobs.
 * * @copyright
 * Copyright (c) 2026 Kenneth Nielson. All rights reserved.
 * Licensed under the OpenPBS v2.3 Software License.
 */

#pragma once

#include "Libnet/AsyncConnection.hpp"
#include "proto/tm_messages.pb.h"
#include <boost/asio.hpp>
#include <string>
#include <vector>

namespace asio = boost::asio;
using torque_ng::net::AsyncConnection;
using namespace torque_ng::tm;

namespace torque_ng::mom {

class SisterCoordinator {
  asio::io_context &io_ctx_;

public:
  explicit SisterCoordinator(asio::io_context &ctx) : io_ctx_(ctx) {}

  /**
   * @brief Spawns tasks across multiple sister nodes in parallel.
   * * @param sister_hosts List of hostnames/IPs for sister nodes.
   * @param base_request The template spawn request to send.
   * @return asio::awaitable<bool> True if all sisters reported success.
   */
  asio::awaitable<bool>
  coordinate_spawn(const std::vector<std::string> &sister_hosts,
                   const SpawnTaskRequest &base_request) {
    // Use a shared atomic or a counter to track successes
    auto shared_success = std::make_shared<std::atomic<bool>>(true);
    auto remaining = std::make_shared<std::atomic<size_t>>(sister_hosts.size());

    for (const auto &host : sister_hosts) {
      // Launch each sister contact as its own independent coroutine
      asio::co_spawn(
          io_ctx_,
          contact_sister(host, base_request, shared_success, remaining),
          asio::detached);
    }

    // Wait (non-blocking) until all coroutines report back
    while (remaining->load() > 0) {
      // Yield control so other tasks can run while sisters respond
      co_await asio::post(io_ctx_, asio::use_awaitable);
    }

    co_return shared_success->load();
  }

private:
  asio::awaitable<void>
  contact_sister(std::string host, SpawnTaskRequest req,
                 std::shared_ptr<std::atomic<bool>> global_success,
                 std::shared_ptr<std::atomic<size_t>> remaining) {
    try {
      asio::ip::tcp::resolver resolver(io_ctx_);
      auto endpoints =
          co_await resolver.async_resolve(host, "15001", asio::use_awaitable);

      asio::ip::tcp::socket socket(io_ctx_);
      co_await asio::async_connect(socket, endpoints, asio::use_awaitable);

      AsyncConnection conn(std::move(socket));

      // Wrap in an envelope
      TmRequest envelope;
      *envelope.mutable_spawn_task() = req;

      co_await conn.send_message(envelope);

      TmResponse response;
      co_await conn.receive_message(response);

      if (!response.success()) {
        *global_success = false;
      }
    } catch (const std::exception &e) {
      *global_success = false;
    }

    (*remaining)--;
    co_return;
  }
};

} // namespace torque_ng::mom