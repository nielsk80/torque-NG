/*
 * torque-NG: Next Generation Resource Manager
 *
 * Copyright (c) 2026 Kenneth Nielson.
 * Licensed under the OpenPBS v2.3 Software License.
 */

/**
 * @file AsyncConnection.hpp
 * @brief Coroutine-based wrapper for Protobuf-over-TCP communication.
 */

#pragma once

#include <boost/asio.hpp>
#include <cstdint>
#include <google/protobuf/message.h>
#include <vector>

namespace asio = boost::asio;
using asio::ip::tcp;

namespace torque_ng::net {

class AsyncConnection {
  tcp::socket socket_;

public:
  explicit AsyncConnection(tcp::socket socket) : socket_(std::move(socket)) {}

  /**
   * @brief Sends a Protobuf message asynchronously with a length prefix.
   */
  asio::awaitable<void> send_message(const google::protobuf::Message &message) {
    // 1. Serialize to a string
    std::string payload;
    if (!message.SerializeToString(&payload)) {
      throw std::runtime_error("Failed to serialize Protobuf message");
    }

    // 2. Create a single buffer containing both Header and Payload
    // We use a vector or a string that will stay alive during the co_await
    uint32_t len_h = htonl(static_cast<uint32_t>(payload.size()));

    std::string full_packet;
    full_packet.reserve(sizeof(len_h) + payload.size());

    // Append 4-byte header
    full_packet.append(reinterpret_cast<const char *>(&len_h), sizeof(len_h));
    // Append payload
    full_packet.append(payload);

    // 3. Write the consolidated packet
    // Asio's use_awaitable will keep full_packet alive if it's a local
    // coroutine variable, but the scatter-gather pointers were likely the
    // source of the SegFault.
    co_await asio::async_write(socket_, asio::buffer(full_packet),
                               asio::use_awaitable);
  }

  /**
   * @brief Reads a Protobuf message asynchronously.
   */
  asio::awaitable<void> receive_message(google::protobuf::Message &message) {
    // 1. Read the 4-byte length header
    uint32_t len_nbo = 0;
    co_await asio::async_read(socket_, asio::buffer(&len_nbo, sizeof(len_nbo)),
                              asio::use_awaitable);
    uint32_t len = ntohl(len_nbo);

    // 2. Read the actual payload
    std::string payload;
    payload.resize(len);
    co_await asio::async_read(socket_, asio::buffer(payload),
                              asio::use_awaitable);

    // 3. Parse back into the Protobuf object
    if (!message.ParseFromString(payload)) {
      throw std::runtime_error("Failed to parse incoming Protobuf message");
    }
  }

  tcp::socket &socket() { return socket_; }
};

} // namespace torque_ng::net