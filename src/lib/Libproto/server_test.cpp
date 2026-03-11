/*
 * torque-NG: Next Generation Resource Manager
 *
 * Copyright (c) 2026 Kenneth Nielson.
 * Portions Copyright (c) 1999-2000 Veridian Information Solutions, Inc.
 * Licensed under the OpenPBS v2.3 Software License.
 */

#include "NetworkManager.hpp"
#include "Job.hpp"
#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>

int main() {
    torque_ng::NetworkManager net;
    int port = 15001;
    int listen_fd = net.create_listener(port);

    if (listen_fd < 0) {
        std::cerr << "Failed to create listener on port " << port << std::endl;
        return 1;
    }

    std::cout << "Server: Listening on port " << port << "..." << std::endl;

    // 1. Accept a connection
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    int client_fd = accept(listen_fd, (struct sockaddr*)&client_addr, &addr_len);

    if (client_fd < 0) {
        std::cerr << "Accept failed" << std::endl;
        return 1;
    }

    std::cout << "Server: Connection accepted." << std::endl;

    // 2. Read the 4-byte length prefix
    uint32_t network_len = 0;
    if (torque_ng::NetworkManager::read_exact(client_fd, reinterpret_cast<char*>(&network_len), 4)) {
        uint32_t host_len = ntohl(network_len);
        std::cout << "Server: Expecting " << host_len << " bytes of Protobuf data." << std::endl;

        // 3. Read the actual payload
        std::string buffer(host_len, '\0');
        if (torque_ng::NetworkManager::read_exact(client_fd, &buffer[0], host_len)) {
            
            // 4. Deserialize back into a Job object
            auto received_job = torque_ng::Job::deserialize(buffer);
            if (received_job) {
                std::cout << "Server: Successfully received Job ID: " << received_job->get_id() << std::endl;
            }
        }
    }

    close(client_fd);
    close(listen_fd);
    return 0;
}