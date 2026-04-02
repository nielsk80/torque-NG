/**
 * @file JobOutputHandler.hpp
 * @brief Manages the creation and writing of temporary and final job output
 * files.
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
#include "include/log.h" // Switched to modern log interface
#include <boost/asio.hpp>
#include <filesystem>
#include <fstream>
#include <memory>

namespace asio = boost::asio;
namespace fs = std::filesystem;

namespace torque_ng::mom {

class JobOutputHandler {
  fs::path spool_dir_;
  std::string job_id_;

public:
  JobOutputHandler(const std::string &job_id,
                   fs::path spool_dir = "/var/spool/torque/spool/")
      : job_id_(job_id), spool_dir_(spool_dir) {}

  /**
   * @brief Opens a temporary spool file with security validation.
   * @param suffix The suffix (.OU or .ER)
   */
  std::unique_ptr<std::ofstream> open_spool_file(const std::string &suffix) {
    fs::path spool_path = spool_dir_ / (job_id_ + suffix);

    // Utilize your chk_file_sec.cpp logic
    // 0022 mask disallows world-write and group-write permissions
    int sec_check = validate_path_security(spool_dir_, true, 0022);

    if (sec_check != 0) {
      // FIX: Using the modern torque_ng::log_event which accepts enum classes
      torque_ng::log_event(Torque::EventType::Error, Torque::EventClass::Job,
                           job_id_,
                           "Security validation failed for spool directory: " +
                               spool_dir_.string());
      return nullptr;
    }

    auto file = std::make_unique<std::ofstream>(spool_path, std::ios::app);
    if (!file->is_open()) {
      torque_ng::log_event(Torque::EventType::Error, Torque::EventClass::Job,
                           job_id_,
                           "Failed to open spool file: " + spool_path.string());
    }
    return file;
  }
};

} // namespace torque_ng::mom