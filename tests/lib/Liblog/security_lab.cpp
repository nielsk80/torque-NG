#include <iostream>
#include <filesystem>
#include <vector>
#include <thread>
#include <string>
#include <mutex>
#include <atomic>
#include <fstream>
#include <log.h>
#include <sys/stat.h>
#include <unistd.h>

namespace fs = std::filesystem;

// Global to track if we found any race conditions or logic errors
std::atomic<int> failure_count{0};

void stress_worker(int thread_id, const fs::path& secure_path, const fs::path& insecure_path) {
    std::string SEMsg;
    
    // Each thread runs 100 iterations to stress the log_mutex
    for (int i = 0; i < 100; ++i) {
        // 1. Test a known secure path
        int rc = chk_file_sec(secure_path, false, true, S_IWGRP | S_IWOTH, true, SEMsg);
        if (rc != 0) {
            failure_count++;
        }

        // 2. Test a known insecure path (this will trigger report_security_error)
        // This stresses the std::ostringstream and log_err inside your library
        rc = chk_file_sec(insecure_path, true, true, S_IWGRP | S_IWOTH, true, SEMsg);
        if (rc != EACCES) {
            failure_count++;
        }
    }
}

int main() {
    if (getuid() != 0) {
        std::cerr << "Stress test must run as root for UID checks.\n";
        return 1;
    }

    fs::path lab = "./stress_zone";
    fs::create_directories(lab);

    // Setup a secure file
    fs::path secure_file = lab / "secure.txt";
    std::basic_ofstream<char>(secure_file) << "safe";

    // Setup an insecure directory (world writable)
    fs::path insecure_dir = lab / "insecure_dir";
    fs::create_directory(insecure_dir);
    fs::permissions(insecure_dir, fs::perms::others_write, fs::perm_options::add);

    const int thread_count = 28; // Matching your Core 7 250H capacity
    std::vector<std::thread> workers;

    std::cout << "Launching " << thread_count << " threads for security stress test...\n";

    for (int i = 0; i < thread_count; ++i) {
        workers.emplace_back(stress_worker, i, secure_file, insecure_dir);
    }

    for (auto& t : workers) {
        t.join();
    }

    if (failure_count == 0) {
        std::cout << "\nSUCCESS: All threads completed without race conditions.\n";
    } else {
        std::cout << "\nFAILURE: " << failure_count << " errors detected during stress test.\n";
    }

    fs::remove_all(lab);
    return 0;
}