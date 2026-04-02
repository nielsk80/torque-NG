/**
 * @file ProcessManager.hpp
 * @brief Handles asynchronous process spawning and monitoring for local tasks.
 * * Copyright (c) 2026 Kenneth Nielson.
 * Licensed under the OpenPBS v2.3 Software License.
 */

#pragma once

#include "Liblog/LogTypes.hpp" // For EventType and EventClass
#include "Liblog/log_event.h"  // For log_event()
#include "proto/tm_messages.pb.h"
#include <boost/asio.hpp>
#include <boost/process.hpp>

namespace asio = boost::asio;
namespace bp = boost::process;

namespace torque_ng::mom {

class ProcessManager {
  asio::io_context &io_ctx_;

public:
  explicit ProcessManager(asio::io_context &ctx) : io_ctx_(ctx) {}

  /**
   * @brief Spawns a local task and monitors it asynchronously.
   */
  asio::awaitable<int>
  async_spawn_and_watch(const torque_ng::tm::SpawnTaskRequest &req) {
    std::string job_id = req.job_id();

    try {
      bp::async_pipe out_pipe(io_ctx_);
      bp::async_pipe err_pipe(io_ctx_);

      // 1. Log the Spawning attempt using your Liblog bridge
      log_event(PBSEVENT_JOB, PBS_EVENTCLASS_JOB, job_id,
                "ProcessManager: Spawning task " + req.executable_path());

      bp::child c(req.executable_path(), bp::args(req.arguments()),
                  bp::env(req.env_vars()), bp::std_out > out_pipe,
                  bp::std_err > err_pipe, io_ctx_);

      // 2. Pass the job_id to the drain coroutines for contextual logging
      asio::co_spawn(io_ctx_, drain_pipe(std::move(out_pipe), "STDOUT", job_id),
                     asio::detached);
      asio::co_spawn(io_ctx_, drain_pipe(std::move(err_pipe), "STDERR", job_id),
                     asio::detached);

      co_await c.async_wait(asio::use_awaitable);

      int exit_code = c.exit_code();

      // 3. Log the successful completion
      log_event(PBSEVENT_JOB, PBS_EVENTCLASS_JOB, job_id,
                "ProcessManager: Task finished with exit code " +
                    std::to_string(exit_code));

      co_return exit_code;

    } catch (const std::exception &e) {
      // 4. Log the failure as an ERROR event
      log_event(PBSEVENT_ERROR, PBS_EVENTCLASS_JOB, job_id,
                "ProcessManager: Failed to launch task: " +
                    std::string(e.what()));
      co_return -1;
    }
  }

private:
  asio::awaitable<void> drain_pipe(bp::async_pipe pipe,
                                   std::shared_ptr<std::ofstream> output_file) {
    char data[8192]; // Larger buffer for job throughput
    for (;;) {
      try {
        std::size_t n = co_await pipe.async_read_some(asio::buffer(data),
                                                      asio::use_awaitable);
        if (n == 0)
          break;

        if (output_file && output_file->is_open()) {
          output_file->write(data, n);
          output_file->flush(); // Ensure data hits disk for qpeek compatibility
        }
      } catch (const std::system_error &e) {
        if (e.code() == asio::error::eof)
          break;
        throw;
      }
    }
  }
};

} // namespace torque_ng::mom