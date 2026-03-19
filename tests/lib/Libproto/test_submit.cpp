#include <gtest/gtest.h>
#include "Dispatcher.hpp"
#include "Job.hpp"
#include "torque_messages.pb.h"

TEST(LibprotoTest, HandleSubmitRequest) {
    MessageDispatcher dispatcher;

    dispatcher.registerSubmitHandler([](const torque_ng::JobSubmitRequest& req) {
        // 1. Create the new Job object (Simulating server-side creation)
        torque_ng::Job new_job("1.localhost");
        new_job.set_owner(req.user_name());
        
        // 2. Map resource strings to the Job's numeric limits
        // (Simple example: mapping 'nodes' from the map)
        auto it = req.resources().find("nodes");
        uint32_t node_count = (it != req.resources().end()) ? std::stoi(it->second) : 1;
        new_job.set_resource_limits(0, 0, node_count, 0, 0);

        // 3. Prepare the success reply
        torque_ng::TorqueReply reply;
        reply.set_error_code(0);
        reply.set_error_message("Job 1.localhost submitted successfully");
        return reply;
    });

    // Simulate incoming 'qsub' request
    torque_ng::TorqueRequest request;
    auto* sub = request.mutable_submit();
    sub->set_job_name("ComputePi");
    sub->set_user_name("knielson");
    (*sub->mutable_resources())["nodes"] = "4";

    torque_ng::TorqueReply result = dispatcher.route(request);

    EXPECT_EQ(result.error_code(), 0);
    EXPECT_TRUE(result.error_message().find("1.localhost") != std::string::npos);
}
