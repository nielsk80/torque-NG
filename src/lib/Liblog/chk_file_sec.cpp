#include "chk_file_sec.h"
#include <iostream>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

bool IamRoot() {
    return (geteuid() == 0);
}

int validate_path_security(const fs::path& path, bool is_dir, int disallow_mask, bool sticky_check) {
    std::error_code ec;
    fs::path target = path;
    bool exists = fs::exists(path, ec);

    // 1. If the file/dir doesn't exist, validate the parent directory instead
    if (!exists) {
        target = path.parent_path();
        if (target.empty()) target = "."; // Default to current dir if no path provided
        
        if (!fs::exists(target, ec)) {
            return ENOENT; // Parent directory must exist
        }
        // Force directory-style checks on the parent
        is_dir = true; 
    }

    struct stat st;
    if (stat(target.c_str(), &st) != 0) {
        return errno;
    }

    // 2. Ownership Check: Root or Daemon User
    if (st.st_uid != 0 && st.st_uid != geteuid()) {
        std::cerr << "torque-ng [SECURITY]: " << target << " owned by unauthorized UID " << st.st_uid << std::endl;
        return EPERM;
    }

    // 3. Permission Validation
    int current_mode = st.st_mode & 0777;
    if ((current_mode & disallow_mask) != 0) {
        std::cerr << "torque-ng [SECURITY]: " << target << " has insecure bits: " 
                  << std::oct << (current_mode & disallow_mask) << std::dec << std::endl;
        return EACCES;
    }

    return 0; // Success: Path is safe to use or create within
}