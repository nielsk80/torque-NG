/*
 * torque-NG: Next Generation Resource Manager
 *
 * Copyright (c) 2026 Kenneth Nielson.
 * Portions Copyright (c) 1999-2000 Veridian Information Solutions, Inc.
 * Licensed under the OpenPBS v2.3 Software License.
 */

#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace torque_ng {

/**
 * @class NetworkManager
 * @brief Handles low-level socket operations and message framing for torque-NG.
 * * This class replaces the legacy C-based networking in net_server.c and
 * net_client.c. It implements a length-prefixed framing protocol to ensure
 * Protobuf messages are received in their entirety over a TCP stream.
 */
class NetworkManager {
public:
  NetworkManager() = default;

  /**
   * @brief Prepends a 4-byte network-byte-order length prefix to a payload.
   * * This creates a "frame" around the Protobuf data. The receiver reads the
   * first 4 bytes to determine how much data to expect for the full message.
   * * @param payload The serialized Protobuf string.
   * @return A vector of bytes containing [Length(4 bytes)][Payload(N bytes)].
   */
  static std::vector<uint8_t> frame_message(const std::string &payload);

  /**
   * @brief Blocking read that ensures an exact number of bytes are retrieved.
   * * Unlike raw read(), which may return partial data, this function loops
   * until the requested length is fulfilled or the connection fails.
   * * @param socket The active file descriptor.
   * @param buffer The destination buffer for the data.
   * @param length The exact number of bytes to read.
   * @return true if all bytes were read, false if the connection
   * closed/errored.
   */
  static bool read_exact(int socket, char *buffer, size_t length);

  /**
   * @brief Initializes a TCP listener socket.
   * @param port The port number to bind to.
   * @return The socket file descriptor, or -1 on failure.
   */
  int create_listener(int port);
};

} // namespace torque_ng