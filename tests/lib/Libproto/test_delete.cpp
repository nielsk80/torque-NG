/*
 * torque-NG: Next Generation Resource Manager
 *
 * Copyright (c) 2026 Kenneth Nielson.
 * Portions Copyright (c) 1999-2000 Veridian Information Solutions, Inc.
 * Licensed under the OpenPBS v2.3 Software License.
 */

#include "Dispatcher.hpp"
#include "torque_ng.pb.h"
#include <gtest/gtest.h>

TEST(LibprotoTest, DeleteJobHandling) {
  MessageDispatcher dispatcher;
  bool handler_called = false;

  // 1. Define the Server-Side logic for Job Delete
  dispatcher.registerDeleteHandler([&](const torque_ng::JobDeleteRequest &req) {
    handler_called = true;

    // Verify the incoming Protobuf data
    EXPECT_EQ(req.job_id(), "123.localhost");
    EXPECT_EQ(req.reason(), "User requested cancellation");
    EXPECT_TRUE(req.force());

    // Return a successful response
    torque_ng::TorqueReply reply;
    reply.set_error_code(0);
    reply.set_error_message("Job " + req.job_id() + " marked for deletion.");
    return reply;
  });

  // 2. Simulate an incoming Request (e.g., from qdel)
  torque_ng::TorqueRequest incoming_request;
  auto *delete_payload = incoming_request.mutable_delete_();
  delete_payload->set_job_id("123.localhost");
  delete_payload->set_reason("User requested cancellation");
  delete_payload->set_force(true);

  // 3. Dispatch the message
  torque_ng::TorqueReply server_reply = dispatcher.route(incoming_request);

  // 4. Verify results
  EXPECT_TRUE(handler_called);
  EXPECT_EQ(server_reply.error_code(), 0);
  EXPECT_EQ(server_reply.error_message(),
            "Job 123.localhost marked for deletion.");
}

// Additional test to ensure 'force' defaults to false if not set
TEST(LibprotoTest, DeleteJobDefaults) {
  MessageDispatcher dispatcher;

  dispatcher.registerDeleteHandler([](const torque_ng::JobDeleteRequest &req) {
    EXPECT_FALSE(req.force()); // Verify Protobuf default behavior

    torque_ng::TorqueReply reply;
    reply.set_error_code(0);
    return reply;
  });

  torque_ng::TorqueRequest request;
  request.mutable_delete_()->set_job_id("456.localhost");

  dispatcher.route(request);
}