/**
 * @file JobStager.hpp
 * @brief Asynchronously handles the movement of spool files to final
 * destinations.
 *
 * Copyright (c) 2026 Kenneth Nielson.
 * Portions Copyright (c) 1999-2000 Veridian Information Solutions, Inc.
 * All rights reserved.
 *
 * Licensed under the OpenPBS v2.3 Software License.
 * See the LICENSE file in the project root for full license details.
 *
 * SPDX-License-Identifier: OpenPBS-2.3
 */

#pragma once

#include "Liblog/chk_file_sec.h"
#include "include/log.h"
#include <boost/asio.hpp>
#include <filesystem>
#include <system_error>

namespace asio = boost::asio;
namespace fs = std::filesystem;

namespace torque_ng::mom {

class JobStager {
  asio::io_context &io_ctx_;

public:
  explicit JobStager(asio::io_context &ctx) : io_ctx_(ctx) {}

  /**
   * @brief Stages out a single spool file to its final destination.
   * @param job_id The ID of the job being processed.
   * @param spool_path The temporary file (.OU or .ER).
   * @param final_dest The destination file (.o or .e).
   */
  asio::awaitable<bool> async_stage_out(std::string job_id, fs::path spool_path,
                                        fs::path final_dest) {
    try {
      // 1. Offload to the thread pool to avoid blocking the coroutine executor
      co_await asio::post(io_ctx_, asio::use_awaitable);

      if (!fs::exists(spool_path)) {
        co_return true; // Nothing to move, treat as success
      }

      // 2. Security Audit: Ensure the destination directory is safe
      // Disallow world-writable destinations for root-owned moves
      int sec_check =
          validate_path_security(final_dest.parent_path(), true, 0022);
      if (sec_check != 0) {
        torque_ng::log_event(
            Torque::EventType::Error, Torque::EventClass::Mom, job_id,
            "Staging failed: Destination directory security check failed.");
        co_return false;
      }

      // 3. The Move Operation
      std::error_code ec;
      fs::rename(spool_path, final_dest, ec);

      if (ec) {
        // EXDEV: Rename failed because files are on different filesystems
        // (e.g., /var to /home/nfs)
        fs::copy(spool_path, final_dest, fs::copy_options::overwrite_existing,
                 ec);
        if (!ec) {
          fs::remove(spool_path, ec);
        }
      }

      if (ec) {
        torque_ng::log_event(
            Torque::EventType::Error, Torque::EventClass::Mom, job_id,
            "Staging failed for " + final_dest.string() + ": " + ec.message());
        co_return false;
      }

      torque_ng::log_event(
          Torque::EventType::Job, Torque::EventClass::Mom, job_id,
          "Successfully staged " + spool_path.filename().string() + " to " +
              final_dest.string());

      co_return true;

    } catch (const std::exception &e) {
      torque_ng::log_event(Torque::EventType::Error, Torque::EventClass::Mom,
                           job_id,
                           "Staging exception: " + std::string(e.what()));
      co_return false;
    }
  }
};

} // namespace torque_ng::mom