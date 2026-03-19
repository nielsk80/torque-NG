#include <iostream>
#include <cassert>
#include "Dispatcher.hpp"
#include "torque_messages.pb.h"

int main() {
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    MessageDispatcher dispatcher;

    // 1. Register the Obituary Handler
    dispatcher.registerObituaryHandler([](const torque_ng::JobObituary& obit) {
        std::cout << "[SERVER] Processing Obituary for: " << obit.job_id() << std::endl;
        std::cout << "[SERVER] Exit Status: " << obit.exit_status() << std::endl;
        std::cout << "[SERVER] Memory Used: " << obit.resources().mem_bytes() / (1024 * 1024) << " MB" << std::endl;

        torque_ng::TorqueReply reply;
        reply.set_error_code(0);
        reply.set_error_message("Obituary recorded");
        return reply;
    });

    // 2. Simulate MOM sending an Obituary
    torque_ng::TorqueRequest request;
    auto* obit = request.mutable_obituary();
    obit->set_job_id("98765.localhost");
    obit->set_exit_status(1); // Simulate failure
    obit->set_node_name("compute-node-01");
    
    auto* res = obit->mutable_resources();
    res->set_mem_bytes(536870912); // 512MB 
    res->set_cpu_time_seconds(120);

    // 3. Dispatch
    std::cout << "--- Dispatching JobObituary ---" << std::endl;
    torque_ng::TorqueReply result = dispatcher.route(request);

    // 4. Verification
    assert(result.error_code() == 0);
    std::cout << "TEST PASSED: " << result.error_message() << std::endl;

    google::protobuf::ShutdownProtobufLibrary();
    return 0;
}
