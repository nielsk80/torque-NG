#include "torque_ng.pb.h"
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>
#include <google/protobuf/util/time_util.h>
#include <gtest/gtest.h>

using google::protobuf::util::TimeUtil;

class SerializationTest : public ::testing::Test {
protected:
  // Helper to simulate a network round-trip
  template <typename T> T round_trip(const T &input) {
    std::string serialized;
    EXPECT_TRUE(input.SerializeToString(&serialized));

    T output;
    EXPECT_TRUE(output.ParseFromString(serialized));
    return output;
  }
};

TEST_F(SerializationTest, VerifyPackedEncodingEfficiency) {
  torque_ng::JobIndexBatch batch;
  const int num_entries = 100;

  // Fill a packed field (priorities is [packed = true])
  for (int i = 0; i < num_entries; ++i) {
    batch.add_priorities(100);
  }

  std::string serialized;
  batch.SerializeToString(&serialized);

  // LOGIC:
  // Tag for field 3 (priorities) is 1 byte.
  // In Proto3, int32 '100' takes 1 byte as a varint.
  // Non-packed size: 100 * (1 tag + 1 value) = 200 bytes.
  // Packed size: 1 tag + 1 length byte + (100 * 1 value) = 102 bytes.

  EXPECT_LT(serialized.size(), 110);
  EXPECT_GT(serialized.size(), 100);

  std::cout << "[ INFO ] Serialized size for 100 packed ints: "
            << serialized.size() << " bytes" << std::endl;
}

// 1. Test ResourceList Map Serialization
TEST_F(SerializationTest, ResourceListPreservesMaps) {
  torque_ng::ResourceList original;

  // Set various resource types
  original.mutable_int_resources()->insert({"ncpus", 16});
  original.mutable_size_resources()->insert(
      {"mem", 32ULL * 1024 * 1024 * 1024});
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
  original.set_job_owner("knielson");
  original.set_queue_name("batch");
  original.set_state(torque_ng::Job::RUNNING);
  original.set_substate(torque_ng::Job::SUBSTATE_RUNNING);
  original.set_priority(50);

  // Resource Maps
  auto *req = original.mutable_resources_requested();
  req->mutable_int_resources()->insert({"ncpus", 16});
  req->mutable_size_resources()->insert({"mem", 32ULL * 1024 * 1024 * 1024});
  req->mutable_string_resources()->insert({"arch", "x86_64"});

  // Set a timestamp
  auto now = TimeUtil::GetCurrentTime();
  *original.mutable_created_at() = now;

  auto recovered = round_trip(original);

  // Scalar fields
  EXPECT_EQ(recovered.job_id(), "1234.localhost");
  EXPECT_EQ(recovered.job_name(), "Compute_Kernels");
  EXPECT_EQ(recovered.job_owner(), "knielson");
  EXPECT_EQ(recovered.queue_name(), "batch");
  EXPECT_EQ(recovered.state(), torque_ng::Job::RUNNING);
  EXPECT_EQ(recovered.substate(), torque_ng::Job::SUBSTATE_RUNNING);
  EXPECT_EQ(recovered.priority(), 50);

  // Map fields (Resources)
  EXPECT_EQ(recovered.resources_requested().int_resources().at("ncpus"), 16);
  EXPECT_EQ(recovered.resources_requested().size_resources().at("mem"),
            32ULL * 1024 * 1024 * 1024);
  EXPECT_EQ(recovered.resources_requested().string_resources().at("arch"),
            "x86_64");

  // Timestamp field
  EXPECT_EQ(recovered.created_at().seconds(), now.seconds());
  EXPECT_EQ(recovered.created_at().nanos(), now.nanos());
}

// 3. Test TorqueReply Error Handling
TEST_F(SerializationTest, ReplyHandlesErrorMessages) {
  torque_ng::TorqueReply original;
  original.set_error_code(-1); // Match your Dispatcher.hpp error code
  original.set_error_message("Access Denied: Invalid Credentials");

  auto recovered = round_trip(original);

  EXPECT_EQ(recovered.error_code(), -1);
  EXPECT_STREQ(recovered.error_message().c_str(),
               "Access Denied: Invalid Credentials");
}