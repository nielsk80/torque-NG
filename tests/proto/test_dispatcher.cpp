#include <gtest/gtest.h>
#include "Dispatcher.hpp"
#include "torque_ng.pb.h"

// ---------------------------------------------------------
// THE FIXTURE
// ---------------------------------------------------------
class TorqueNetworkTest : public ::testing::Test {
protected:
    MessageDispatcher dispatcher;
    bool submit_called = false;
    bool delete_called = false;

    // SetUp runs before every TEST_F
    void SetUp() override {
        // Mock Submit Handler
        dispatcher.registerSubmitHandler([this](const torque_ng::JobSubmitRequest& req) {
            submit_called = true;
            torque_ng::TorqueReply reply;
            reply.set_error_code(0); 
            return reply;
        });

        // Mock Delete Handler
        dispatcher.registerDeleteHandler([this](const torque_ng::JobDeleteRequest& req) {
            delete_called = true;
            torque_ng::TorqueReply reply;
            if (req.job_id().empty()) {
                reply.set_error_code(-1);
                reply.set_error_message("Invalid ID");
            } else {
                reply.set_error_code(0);
            }
            return reply;
        });
    }
};

// ---------------------------------------------------------
// THE TESTS
// ---------------------------------------------------------

TEST_F(TorqueNetworkTest, DispatcherRoutesSubmitCorrectly) {
    torque_ng::TorqueRequest request;
    request.mutable_submit()->mutable_job()->set_job_name("GTest_Job");

    auto reply = dispatcher.route(request);

    EXPECT_TRUE(submit_called);
    EXPECT_EQ(reply.error_code(), 0);
}

TEST_F(TorqueNetworkTest, DispatcherRoutesDeleteCorrectly) {
    torque_ng::TorqueRequest request;
    request.mutable_delete_()->set_job_id("123.localhost");

    auto reply = dispatcher.route(request);

    EXPECT_TRUE(delete_called);
    EXPECT_EQ(reply.error_code(), 0);
}