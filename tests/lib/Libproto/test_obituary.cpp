/*
 * torque-NG: Next Generation Resource Manager
 *
 * Copyright (c) 2026 Kenneth Nielson.
 * Portions Copyright (c) 1999-2000 Veridian Information Solutions, Inc.
 * Licensed under the OpenPBS v2.3 Software License.
 */

#include <gtest/gtest.h>
#include "Dispatcher.hpp"
#include "torque_messages.pb.h"

// The TEST macro defines a test case. 
// LibprotoTest is the Group, ObituaryHandling is the specific test name.
TEST(LibprotoTest, ObituaryHandling) {
    MessageDispatcher dispatcher;
    bool handler_was_called = false;

    // 1. Register the Obituary Handler
    dispatcher.registerObituaryHandler([&](const torque_ng::JobObituary& obit) {
        handler_was_called = true;
        
        // Use GTest assertions instead of std::cout/assert
        EXPECT_EQ(obit.job_id(), "98765.localhost");
        EXPECT_EQ(obit.exit_status(), 1);
        EXPECT_STREQ(obit.node_name().c_str(), "compute-node-01");
        
        // Verify nested resource values
        EXPECT_EQ(obit.resources().mem_bytes(), 536870912);

        torque_ng::TorqueReply reply;
        reply.set_error_code(0);
        reply.set_error_message("Obituary recorded");
        return reply;
    });

    // 2. Simulate MOM sending an Obituary
    torque_ng::TorqueRequest request;
    auto* obit = request.mutable_obituary();
    obit->set_job_id("98765.localhost");
    obit->set_exit_status(1);
    obit->set_node_name("compute-node-01");
    
    auto* res = obit->mutable_resources();
    res->set_mem_bytes(536870912); 
    res->set_cpu_time_seconds(120);

    // 3. Dispatch using the new route() method
    torque_ng::TorqueReply result = dispatcher.route(request);

    // 4. Final Verifications
    EXPECT_TRUE(handler_was_called);
    EXPECT_EQ(result.error_code(), 0);
    EXPECT_EQ(result.error_message(), "Obituary recorded");
}