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

#include "MessageListener.hpp"
#include "MessageFactory.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdexcept>
#include <format>

namespace torque_ng::network {

MessageListener::MessageListener(const NetworkAddress& address) 
    : address_(address) {
    setup_socket(); // snake_case
}

MessageListener::~MessageListener() {
    stop();
    if (listen_fd_ != -1) {
        close(listen_fd_);
    }
}

void MessageListener::setup_socket() {
    listen_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd_ == -1) {
        throw std::runtime_error("Failed to create socket");
    }

    int opt = 1;
    setsockopt(listen_fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    ::sockaddr_in addr = address_.to_sockaddr_in();
    
    if (bind(listen_fd_, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr)) < 0) {
        throw std::runtime_error(std::format("Failed to bind to port {}", address_.port()));
    }

    if (listen(listen_fd_, 128) < 0) {
        throw std::runtime_error("Listen failed");
    }
}

void MessageListener::run(const message_handler& handler) {
    running_ = true;
    while (running_) {
        ::sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);
        
        int client_fd = accept(listen_fd_, reinterpret_cast<struct sockaddr*>(&client_addr), &client_len);
        
        if (client_fd < 0) {
            if (running_) continue; 
            break;
        }

        auto msg = MessageFactory::create_from_socket(client_fd);
        if (msg) {
            handler(std::move(*msg), client_fd);
        }
        close(client_fd); 
    }
}

void MessageListener::stop() {
    running_ = false;
    if (listen_fd_ != -1) {
        shutdown(listen_fd_, SHUT_RDWR);
    }
}

} // namespace torque_ng::network