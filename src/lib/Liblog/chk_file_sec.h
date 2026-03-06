#pragma once

#include <filesystem>
#include <string>

namespace fs = std::filesystem;

/**
 * validate_path_security - Performs a deep security audit of a file or directory.
 * Returns 0 on success, or a POSIX error code (EPERM, EACCES, etc.) on failure.
 */
int validate_path_security(
    const fs::path& path, 
    bool is_dir, 
    int disallow_mask, 
    bool sticky_check = false);

/**
 * IamRoot - Helper to check if the daemon is running with effective root privileges.
 */
bool IamRoot();