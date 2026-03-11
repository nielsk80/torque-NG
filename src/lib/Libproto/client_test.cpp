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
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int main() {
    // 1. Create Job and Serialize
    torque_ng::Job mom_job("1234.ubuntu-desk");
    mom_job.set_owner("ken-nielson");
    mom_job.set_state(2); // Running
    std::string payload = mom_job.serialize();

    // 2. Frame the message (adds 4-byte length)
    auto framed_data = torque_ng::NetworkManager::frame_message(payload);

    // 3. Connect to Server
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(15001);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection Failed" << std::endl;
        return 1;
    }

    // 4. Send the framed data
    send(sock, framed_data.data(), framed_data.size(), 0);
    std::cout << "Client: Sent framed job update (" << framed_data.size() << " bytes)." << std::endl;

    close(sock);
    return 0;
}