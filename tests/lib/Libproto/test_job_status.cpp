// torque-NG: Libproto Status Protocol Unit Test
// Copyright (c) 2026 Kenneth Nielson.
// SPDX-License-Identifier: OpenPBS-2.3

#include "JobStateMapper.hpp"
#include "torque_ng.pb.h"
#include "Dispatcher.hpp"
#include <iostream>
#include <cassert>
#include <vector>

int main() {
    // Verify protobuf library version compatibility
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    MessageDispatcher dispatcher;

    // 1. Register the Status Handler
    // This simulates the server gathering data for multiple jobs
    dispatcher.registerStatusHandler([](const torque_ng::JobStatusRequest& req) {
        torque_ng::TorqueReply reply;
        reply.set_error_code(0);
        
        auto* status_response = reply.mutable_status_data();

        // Create a small vector of dummy data to simulate a loop over a job list
        struct DummyJob { std::string id; std::string name; std::string state; std::string substate; };
        std::vector<DummyJob> jobs = {
            {"101.local", "Compile_Task", "Running", "0"},
            {"102.local", "Link_Task",    "Queued",  "1"},
            {"103.local", "Cleanup",      "Exiting", "2"}
        };

        for (const auto& j : jobs) {
            auto* info = status_response->add_jobs();
            info->set_job_id(j.id);
            info->set_job_name(j.name);
            info->set_job_state(torque_ng::utils::JobStateMapper::from_string(j.state));
            info->set_job_substate(torque_ng::utils::JobStateMapper::sub_from_string(j.substate));
        }

        return reply;
    });

    // 2. Construct and Dispatch Request
    torque_ng::TorqueRequest request;
    request.mutable_status(); // Trigger the 'status' oneof

    std::cout << "--- Testing Bulk Status Dispatch ---" << std::endl;
    torque_ng::TorqueReply result = dispatcher.route(request);

    // 3. Verification
    assert(result.has_status_data());
    const auto& data = result.status_data();
    
    std::cout << "Received " << data.jobs_size() << " jobs from dispatcher." << std::endl;
    
    for (int i = 0; i < data.jobs_size(); ++i) {
        std::cout << "  Job[" << i << "]: " << data.jobs(i).job_id() 
                  << " (" << data.jobs(i).job_state() << ")" << std::endl;
    }

    assert(data.jobs_size() == 3);
    assert(data.jobs(1).job_id() == "102.local");

    std::cout << "\nTEST PASSED: Bulk status mapping verified." << std::endl;

    google::protobuf::ShutdownProtobufLibrary();
    return 0;
}
