#include <gtest/gtest.h>
#include "Dispatcher.hpp"
#include "Job.hpp" 
#include "torque_ng.pb.h"

TEST(LibprotoTest, HandleSubmitRequest) {
    // Note: Ensure MessageDispatcher is defined to handle TorqueReply/TorqueRequest
    MessageDispatcher dispatcher;

    dispatcher.registerSubmitHandler([](const torque_ng::JobSubmitRequest& req) {
        // 1. Access the inner Job message from the Request wrapper
        const auto& job_data = req.job();
        
        // 2. Create the server-side JobEntity
        torque_ng::JobEntity new_job("1.localhost");
        new_job.set_owner(job_data.owner_name());
        
        // 3. Map resource strings from the ResourceList maps
        // Accessing the string_resources map inside resources_requested
        const auto& res_map = job_data.resources_requested().string_resources();
        auto it = res_map.find("nodes");
        
        uint32_t node_count = (it != res_map.end()) ? std::stoul(it->second) : 1;
        new_job.set_resource_limits(0, 0, node_count, 0, 0);

        // 4. Prepare the success reply
        torque_ng::TorqueReply reply;
        reply.set_error_code(0);
        reply.set_error_message("Job 1.localhost submitted successfully");
        return reply;
    });

    // --- Simulate incoming 'qsub' request ---
    torque_ng::TorqueRequest request;
    
    // Set the oneof payload to be a submit request
    auto* sub = request.mutable_submit(); 
    auto* job = sub->mutable_job(); 
    
    // Set basic Job fields
    job->set_job_name("ComputePi");
    job->set_owner_name("knielson");
    
    // Set resources: navigate to the string_resources map
    auto* string_res = job->mutable_resources_requested()->mutable_string_resources();
    (*string_res)["nodes"] = "4";

    // Route the request through the dispatcher
    torque_ng::TorqueReply result = dispatcher.route(request);

    // Verify results
    EXPECT_EQ(result.error_code(), 0);
    EXPECT_NE(result.error_message().find("1.localhost"), std::string::npos);
}