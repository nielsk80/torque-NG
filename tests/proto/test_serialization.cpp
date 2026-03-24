#include <gtest/gtest.h>
#include <google/protobuf/util/time_util.h>
#include "torque_ng.pb.h"

using google::protobuf::util::TimeUtil;

class SerializationTest : public ::testing::Test {
protected:
    // Helper to simulate a network round-trip
    template <typename T>
    T round_trip(const T& input) {
        std::string serialized;
        EXPECT_TRUE(input.SerializeToString(&serialized));
        
        T output;
        EXPECT_TRUE(output.ParseFromString(serialized));
        return output;
    }
};

// 1. Test ResourceList Map Serialization
TEST_F(SerializationTest, ResourceListPreservesMaps) {
    torque_ng::ResourceList original;
    
    // Set various resource types
    original.mutable_int_resources()->insert({"ncpus", 16});
    original.mutable_size_resources()->insert({"mem", 32ULL * 1024 * 1024 * 1024});
    original.mutable_string_resources()->insert({"arch", "x86_64"});

    auto recovered = round_trip(original);

    EXPECT_EQ(recovered.int_resources().at("ncpus"), 16);
    EXPECT_EQ(recovered.size_resources().at("mem"), 34359738368ULL);
    EXPECT_EQ(recovered.string_resources().at("arch"), "x86_64");
}

// 2. Test Full Job Object Serialization
TEST_F(SerializationTest, JobObjectIntegrity) {
    torque_ng::Job original;
    original.set_job_id("1234.localhost");
    original.set_job_name("Compute_Kernels");
    original.set_owner_name("knielson");
    original.set_state(torque_ng::Job::RUNNING);

    // Set a timestamp
    auto now = TimeUtil::GetCurrentTime();
    *original.mutable_created_at() = now;

    auto recovered = round_trip(original);

    EXPECT_EQ(recovered.job_id(), "1234.localhost");
    EXPECT_EQ(recovered.owner_name(), "knielson");
    EXPECT_EQ(recovered.state(), torque_ng::Job::RUNNING);
    EXPECT_EQ(recovered.created_at().seconds(), now.seconds());
}

// 3. Test TorqueReply Error Handling
TEST_F(SerializationTest, ReplyHandlesErrorMessages) {
    torque_ng::TorqueReply original;
    original.set_error_code(-1); // Match your Dispatcher.hpp error code
    original.set_error_message("Access Denied: Invalid Credentials");

    auto recovered = round_trip(original);

    EXPECT_EQ(recovered.error_code(), -1);
    EXPECT_STREQ(recovered.error_message().c_str(), "Access Denied: Invalid Credentials");
}