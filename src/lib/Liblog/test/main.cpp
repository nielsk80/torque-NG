/*
 * torque-NG: Next Generation Resource Manager
 * Verification Test for liblog_ng
 */

#include "pbs_log.h"
#include "safe_log.hpp"
#include "setup_env.h"
#include <iostream>
#include <vector>

int main() {
  std::cout << "Starting torque-NG Library Test..." << std::endl;

  // 1. Test Hostname Resolution (The function we just added)
  log_set_hostname_sharelogging("", "");

  // 2. Test Secure Log Opening
  // This will trigger validate_path_security on your Ubuntu system
  auto err = log_open("test_daemon.log", "logs");
  if (err) {
    std::cerr << "Failed to open log: " << err.message() << std::endl;
  } else {
    std::cout << "Log opened successfully." << std::endl;
  }

  // 3. Test Modernized Logging
  log_record(static_cast<int>(Torque::EventType::System),
             static_cast<int>(Torque::EventClass::Server), "Main",
             "Daemon initialized and logging started.");

  // 4. Test Environment Refactor
  // Create a dummy env file for testing
  std::system("echo 'PATH=/usr/bin' > test.env");
  std::system("echo 'USER=knielson' >> test.env");

  auto modern_env = setup_env("./test.env");
  std::cout << "Loaded " << modern_env.size() << " environment variables."
            << std::endl;

  // 5. Test C-Style Converter (get_c_envp)
  auto legacy_env = get_c_envp(modern_env);
  if (legacy_env.back() == nullptr) {
    std::cout << "Legacy envp is correctly null-terminated." << std::endl;
  }

  std::cout << "Test complete. Check test_daemon.log for output." << std::endl;

  return 0;
}