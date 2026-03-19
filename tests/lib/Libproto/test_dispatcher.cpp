#include <iostream>
#include <cassert>
#include "Dispatcher.hpp"
#include "torque_messages.pb.h"

int main() {
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    // 1. Initialize the Dispatcher
    MessageDispatcher dispatcher;

    // 2. Register a "Delete" Handler (Mocking the Server logic)
    dispatcher.registerDeleteHandler([](const torque_ng::JobDeleteRequest& req) {
        std::cout << "[SERVER] Received Delete Request for Job: " << req.job_id() << std::endl;
        std::cout << "[SERVER] Reason provided: " << req.reason() << std::endl;

        // Create a successful reply
        torque_ng::TorqueReply reply;
        reply.set_error_code(0); // Success
        reply.set_error_message("Job deleted successfully");
        return reply;
    });

    // 3. Simulate an incoming message (e.g., from a socket)
    torque_ng::TorqueRequest incoming_msg;
    auto* del_req = incoming_msg.mutable_delete_(); // Access the 'oneof' field
    del_req->set_job_id("12345.localhost");
    del_req->set_reason("User requested cancellation");

    // 4. Route the message through the Dispatcher
    std::cout << "--- Dispatching JobDelete ---" << std::endl;
    torque_ng::TorqueReply result = dispatcher.route(incoming_msg);

    // 5. Verify the results
    if (result.error_code() == 0) {
        std::cout << "SUCCESS: " << result.error_message() << std::endl;
    } else {
        std::cerr << "FAILED: " << result.error_message() << std::endl;
    }

    google::protobuf::ShutdownProtobufLibrary();

    return 0;
}
