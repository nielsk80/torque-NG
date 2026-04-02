/*
 * torque-NG: Next Generation Resource Manager
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

#include "safe_log.hpp"
#include "chk_file_sec.h"
#include <arpa/inet.h> // For inet_ntop()
#include <chrono>      // Required for the get_timestamp logic
#include <filesystem>  // Required for S_IWOTH and other permission bits
#include <iomanip>
#include <iostream>
#include <netdb.h> // For addrinfo and getaddrinfo()
#include <sstream>
#include <string>
#include <string_view>
#include <sys/socket.h> // For AF_INET and socket structures
#include <sys/stat.h>
#include <unistd.h> // For gethostname()

// Helper to get ISO-8601 timestamps
std::string get_timestamp() {
  auto now = std::chrono::system_clock::now();
  auto in_time_t = std::chrono::system_clock::to_time_t(now);
  std::stringstream ss;
  ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X");
  return ss.str();
}

void SafeLog::record(Torque::EventType type, Torque::EventClass cls,
                     std::string_view name, std::string_view text) {

  // Lock once for the entire scope of this function
  std::scoped_lock lock(m_mutex);

  // 1. Check if we need to rotate before writing
  check_rotation_triggers();

  // 2. Filter Check: Use the member variable directly
  if (!(static_cast<uint32_t>(type) & m_event_mask))
    return;

  // 3. Direct Write: Since we hold m_mutex, we can use the stream safely
  if (m_logstream.is_open()) {
    m_logstream << "[" << get_timestamp() << "] " << "Host: " << m_hostname
                << " | " << "Type: " << static_cast<int>(type) << " | "
                << "Obj: " << name << " | " << text << std::endl;
  }
}

/**
 * open - Opens the log file in append mode.
 * Converts string_view to a temporary std::string to ensure null-termination
 * for fopen.
 */
bool SafeLog::open(const std::filesystem::path &path) {
  std::scoped_lock lock(m_mutex);

  // Call the modernized check from chk_file_sec.cpp
  if (validate_path_security(path, true, S_IWOTH) != 0) {
    std::cerr << "Security Failure: Log path " << path << " is insecure."
              << std::endl;
    return false;
  }

  m_logpath = path;
  if (m_logstream.is_open())
    m_logstream.close();
  m_logstream.open(path, std::ios::app);
  return m_logstream.is_open();
}

void SafeLog::close() {
  std::scoped_lock lock(m_mutex);
  if (m_logstream.is_open())
    m_logstream.close();
}

/**
 * get_size_kb - Thread-safe check of the log file size.
 * Returns the size in Kilobytes (KB).
 */
long SafeLog::get_size_kb() {
  std::scoped_lock lock(m_mutex);

  if (m_logpath.empty() || !std::filesystem::exists(m_logpath)) {
    return 0; // No file or path set
  }

  try {
    return std::filesystem::file_size(m_logpath) / 1024;
  } catch (...) {
    return 0;
  }
}

/**
 * log_set_hostname_sharelogging - Modernized hostname/IP resolution.
 * Now includes a security alert if the host identity cannot be verified.
 */
void log_set_hostname_sharelogging(std::string_view server_name,
                                   std::string_view server_port) {
  char hostnm[1024];
  std::string final_host = "unknown";

  // 1. Determine base hostname
  if (!server_name.empty()) {
    final_host = server_name;
  } else if (gethostname(hostnm, sizeof(hostnm)) == 0) {
    final_host = hostnm;
  }

  // 2. Resolve to IP address
  struct addrinfo hints = {}, *res = nullptr;
  hints.ai_family = AF_INET;

  int status = getaddrinfo(final_host.c_str(), nullptr, &hints, &res);

  if (status == 0 && res != nullptr) {
    struct sockaddr_in *ipv4 = (struct sockaddr_in *)res->ai_addr;
    char ip_buf[INET_ADDRSTRLEN];

    if (inet_ntop(AF_INET, &(ipv4->sin_addr), ip_buf, sizeof(ip_buf))) {
      SafeLog::getInstance().set_hostname(ip_buf);
    }
    freeaddrinfo(res);
  } else {
    // SECURITY ALERT: If we reach here, the daemon doesn't know who it is.
    std::string error_msg =
        "Critical Networking Alert: Unable to resolve local hostname '";
    error_msg += final_host;
    error_msg += "'. Check DNS or /etc/hosts. Error: ";
    error_msg += gai_strerror(status);

    // Send to both the log file and system journal (stderr)
    SafeLog::getInstance().record(Torque::EventType::Security,
                                  Torque::EventClass::Server, "Network",
                                  error_msg);
    std::cerr << "torque-ng [SECURITY]: " << error_msg << std::endl;

    // Fallback to raw hostname so we can at least attempt to log
    SafeLog::getInstance().set_hostname(final_host);
  }
}

std::string SafeLog::get_current_date_string() {
  auto now =
      std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  std::tm tm_struct;
  localtime_r(&now, &tm_struct); // Thread-safe version of localtime

  std::ostringstream oss;
  oss << std::put_time(&tm_struct, "%Y%m%d");
  return oss.str();
}

/**
 * check_rotation_triggers - Decides if it's time to swap files.
 */
void SafeLog::check_rotation_triggers() {
  std::string today = get_current_date_string();

  // We only set m_last_rotation_date on the first run
  if (m_last_rotation_date.empty()) {
    m_last_rotation_date = today;
    return;
  }

  bool day_changed = (m_last_rotation_date != today);
  bool size_exceeded = (get_size_kb() >= m_max_size_kb);

  if (day_changed || size_exceeded) {
    if (rotate()) {
      m_last_rotation_date = today;
    }
  }
}

/**
 * rotate - The physical act of renaming the old log and opening a new one.
 */
bool SafeLog::rotate() {
  // Note: We assume the caller (record) or a scoped_lock here handles
  // concurrency
  if (m_logpath.empty() || !m_logstream.is_open())
    return false;

  m_logstream.close();

  // Archive name: pbs_log.20260306
  std::filesystem::path archive = m_logpath;
  archive += "." + m_last_rotation_date;

  std::error_code ec;
  std::filesystem::rename(m_logpath, archive, ec);

  if (ec) {
    std::cerr << "torque-ng [ERROR]: Failed to rotate log: " << ec.message()
              << std::endl;
  }

  // Re-open fresh log using our secure open method
  return this->open(m_logpath);
}