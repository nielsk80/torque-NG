// user_context.hpp
#pragma once

#include <string>
#include <vector>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

class UserContext {
public:
    UserContext(const std::string& username);

    // Validation
    bool exists() const { return user_exists; }
    
    // Getters
    uid_t get_uid() const { return uid; }
    gid_t get_gid() const { return gid; }
    std::string get_home_dir() const { return home_dir; }
    std::string get_shell() const { return shell; }

    /**
     * Drops the current process privileges to this user.
     * MUST be called in the child process after fork().
     */
    bool apply_to_current_process() const;

private:
    bool user_exists = false;
    uid_t uid = 0;
    gid_t gid = 0;
    std::string name;
    std::string home_dir;
    std::string shell;

    void lookup_user();
};