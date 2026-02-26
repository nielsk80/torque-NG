// user_context.cpp
#include "user_context.hpp"
#include "pbs_log.h"
#include <vector>
#include <grp.h>

UserContext::UserContext(const std::string& username) : name(username) {
    lookup_user();
}

void UserContext::lookup_user() {
    long bufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
    if (bufsize == -1) bufsize = 16384; 

    std::vector<char> buffer(bufsize);
    struct passwd pwd;
    struct passwd *result;

    if (getpwnam_r(name.c_str(), &pwd, buffer.data(), buffer.size(), &result) == 0 && result != nullptr) {
        uid = pwd.pw_uid;
        gid = pwd.pw_gid;
        home_dir = pwd.pw_dir;
        shell = pwd.pw_shell;
        user_exists = true;
    } else {
        log_event(PBSEVENT_ERROR, PBS_EVENTCLASS_SERVER, "Security", ("User lookup failed: " + name).c_str());
    }
}

bool UserContext::apply_to_current_process() const {
    if (!user_exists) return false;

    // The following three steps must be done in order to properly drop privileges:
    // 1. Set secondary groups for the user
    if (initgroups(name.c_str(), gid) != 0) {
        log_event(PBSEVENT_ERROR, PBS_EVENTCLASS_SERVER, "Security", "Failed to initgroups");
        return false;
    }

    // 2. Set primary Group ID
    if (setgid(gid) != 0) {
        log_event(PBSEVENT_ERROR, PBS_EVENTCLASS_SERVER, "Security", "Failed to setgid");
        return false;
    }

    // 3. Set User ID
    if (setuid(uid) != 0) {
        log_event(PBSEVENT_ERROR, PBS_EVENTCLASS_SERVER, "Security", "Failed to setuid");
        return false;
    }

    // Final sanity check: verify we are no longer root
   if (getuid() != uid || geteuid() != uid) {
      log_event(PBSEVENT_ERROR, PBS_EVENTCLASS_SERVER, "Security", "ID verification failed after setuid");
      return false;
   }

    return true;
}