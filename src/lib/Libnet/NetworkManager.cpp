/*
 * torque-NG: Next Generation Resource Manager
 *
 * Copyright (c) 2026 Kenneth Nielson.
 * Portions Copyright (c) 1999-2000 Veridian Information Solutions, Inc.
 * Licensed under the OpenPBS v2.3 Software License.
 */

#include "NetworkManager.hpp"
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>

namespace torque_ng {

/**
 * @brief Prepends a 4-byte network-byte-order length prefix to a payload.
 * Essential for the Client/MOM to send complete frames.
 */
std::vector<uint8_t> NetworkManager::frame_message(const std::string &payload) {
  uint32_t len = htonl(static_cast<uint32_t>(payload.size()));

  std::vector<uint8_t> framed;
  framed.reserve(sizeof(len) + payload.size());

  uint8_t *len_bytes = reinterpret_cast<uint8_t *>(&len);
  framed.insert(framed.end(), len_bytes, len_bytes + sizeof(len));
  framed.insert(framed.end(), payload.begin(), payload.end());

  return framed;
}

/**
 * @brief Reads an exact number of bytes from a socket.
 * Essential for the Server to reconstruct Protobuf messages.
 */
bool NetworkManager::read_exact(int socket, char *buffer, size_t length) {
  size_t total_read = 0;
  while (total_read < length) {
    ssize_t n = read(socket, buffer + total_read, length - total_read);
    if (n <= 0)
      return false;
    total_read += n;
  }
  return true;
}

/**
 * @brief Initializes a TCP listener socket.
 */
int NetworkManager::create_listener(int port) {
  int server_fd;
  struct sockaddr_in address;
  int opt = 1;

  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    return -1;
  }

  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
                 sizeof(opt))) {
    close(server_fd);
    return -1;
  }

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(port);

  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    close(server_fd);
    return -1;
  }

  if (listen(server_fd, 3) < 0) {
    close(server_fd);
    return -1;
  }

  return server_fd;
}

} // namespace torque_ng