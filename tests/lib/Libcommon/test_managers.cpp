#include "JobIndexManager.hpp"
#include "JobQueueManager.hpp"
#include "MmapBuffer.hpp"
#include "NodeStatusManager.hpp"
#include <filesystem>
#include <gtest/gtest.h>

using namespace torque_ng;
namespace fs = std::filesystem;

// Fixture to handle setup and cleanup of test binary files
class ManagerTest : public ::testing::Test {
protected:
  void SetUp() override {
    // Clean up any stale files from previous runs
    tear_down_files();
  }

  void TearDown() override { tear_down_files(); }

  void tear_down_files() {
    if (fs::exists("test_nodes.bin"))
      fs::remove("test_nodes.bin");
    if (fs::exists("test_queues.bin"))
      fs::remove("test_queues.bin");
    if (fs::exists("test_jobs.bin"))
      fs::remove("test_jobs.bin");
  }
};

// 1. Test NodeStatusManager
TEST_F(ManagerTest, TestNodeStatusUpdate) {
  NodeStatusManager node_mgr("test_nodes.bin", 100);

  Node n;
  n.set_name("compute-001");
  n.set_state(Node::FREE);
  n.set_pcpus(64);

  // Verify update doesn't throw and potentially check internal state if API
  // allows
  EXPECT_NO_THROW(node_mgr.update_node(n));
}

// 2. Test JobQueueManager
TEST_F(ManagerTest, TestJobQueueUpdate) {
  JobQueueManager queue_mgr("test_queues.bin", 10);

  Queue q;
  q.set_name("batch");
  q.set_max_running(50);
  q.set_priority(100);

  EXPECT_NO_THROW(queue_mgr.update_queue(q));
}

// 3. Test JobIndexManager
TEST_F(ManagerTest, TestJobIndexUpdate) {
  JobIndexManager job_mgr("test_jobs.bin", 1000);

  Job j;
  j.set_job_id("123.server");
  j.set_state(Job::QUEUED);
  j.set_priority(50);

  // Standardized field name check
  j.set_job_owner("ken-nielson");

  EXPECT_NO_THROW(job_mgr.update_job(j));
}

// Optional: Test memory mapping persistence
TEST_F(ManagerTest, TestPersistence) {
  {
    JobIndexManager job_mgr("test_jobs.bin", 1000);
    Job j;
    j.set_job_id("456.server");
    j.set_job_owner("ken-nielson");
    job_mgr.update_job(j);
  }

  // Re-open and verify no crashes (simple persistence check)
  EXPECT_NO_THROW({ JobIndexManager job_mgr2("test_jobs.bin", 1000); });
}