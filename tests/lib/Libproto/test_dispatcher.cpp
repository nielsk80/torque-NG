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

// Test Case 1: Verify successful registration and routing of a Delete request
TEST(DispatcherTest, RouteDeleteRequest) {
  MessageDispatcher dispatcher;
  bool handler_called = false;

  // 1. Register a Delete Handler
  dispatcher.registerDeleteHandler([&](const torque_ng::JobDeleteRequest &req) {
    handler_called = true;
    EXPECT_EQ(req.job_id(), "12345.localhost");
    EXPECT_EQ(req.reason(), "User requested cancellation");

    torque_ng::TorqueReply reply;
    reply.set_error_code(0);
    reply.set_error_message("Job deleted successfully");
    return reply;
  });

  // 2. Create the incoming message
  torque_ng::TorqueRequest incoming_msg;
  auto *del_req = incoming_msg.mutable_delete_();
  del_req->set_job_id("12345.localhost");
  del_req->set_reason("User requested cancellation");

  // 3. Route and Verify
  torque_ng::TorqueReply result = dispatcher.route(incoming_msg);

  EXPECT_TRUE(handler_called);
  EXPECT_EQ(result.error_code(), 0);
  EXPECT_STREQ(result.error_message().c_str(), "Job deleted successfully");
}

// Test Case 2: Verify behavior when no handler is registered for a command
TEST(DispatcherTest, UnregisteredHandlerReturnsError) {
  MessageDispatcher dispatcher;

  // Create a request (Submit) but don't register a handler for it
  torque_ng::TorqueRequest incoming_msg;
  incoming_msg.mutable_submit()->mutable_job()->set_job_name("TestJob");

  torque_ng::TorqueReply result = dispatcher.route(incoming_msg);

  // Should return the error defined in Dispatcher.hpp
  EXPECT_EQ(result.error_code(), -1);
  EXPECT_EQ(result.error_message(), "Submit handler not registered");
}

// Test Case 3: Verify behavior with an empty payload
TEST(DispatcherTest, EmptyPayloadReturnsError) {
  MessageDispatcher dispatcher;
  torque_ng::TorqueRequest empty_msg;

  torque_ng::TorqueReply result = dispatcher.route(empty_msg);

  EXPECT_EQ(result.error_code(), -1);
  EXPECT_EQ(result.error_message(), "Empty request payload received");
}