// torque-NG: Libproto Status Protocol Unit Test
// Copyright (c) 2026 Kenneth Nielson.
// SPDX-License-Identifier: OpenPBS-2.3

#include <iostream>
#include <cassert>
#include "Dispatcher.hpp"
#include "torque_messages.pb.h"

int main() {
    MessageDispatcher dispatcher;

    // 1. Define the "Server-Side" logic for Job Delete
    // In a real pbs_server, this would look up the job in the JobManager 
    // and signal the MOM or update the database.
    dispatcher.registerDeleteHandler([](const torque_ng::JobDeleteRequest& req) {
        std::cout << "[Server] Processing Delete for Job: " << req.job_id() << std::endl;
        if (!req.reason().empty()) {
            std::cout << "[Server] Reason provided: " << req.reason() << std::endl;
        }
        if (req.force()) {
            std::cout << "[Server] Force flag detected (SIGKILL path)" << std::endl;
        }

        // Return a successful response
        torque_ng::TorqueReply reply;
        reply.set_error_code(0); // Success
        reply.set_error_message("Job " + req.job_id() + " marked for deletion.");
        return reply;
    });

    // 2. Simulate an incoming "Network" Request (qdel 123.localhost)
    torque_ng::TorqueRequest incoming_request;
    auto* delete_payload = incoming_request.mutable_delete_();
    delete_payload->set_job_id("123.localhost");
    delete_payload->set_reason("User requested cancellation");
    delete_payload->set_force(true);

    // 3. Dispatch the message
    std::cout << "Dispatching JobDeleteRequest..." << std::endl;
    torque_ng::TorqueReply server_reply = dispatcher.route(incoming_request);

    // 4. Verify the result
    std::cout << "Server Reply: [" << server_reply.error_code() << "] " 
              << server_reply.error_message() << std::endl;

    assert(server_reply.error_code() == 0);
    
    return 0;
}
