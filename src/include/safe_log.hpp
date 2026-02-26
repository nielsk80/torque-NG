#ifndef SAFE_LOG_HPP
#define SAFE_LOG_HPP

#include <cstdio>
#include <mutex>
#include <string>
#include <sys/stat.h>

class SafeLog {
private:
    FILE* m_logfile = nullptr;
    std::mutex m_mutex;

public:
    SafeLog() = default;
    
    // Rule of Five: Prevent copying a system resource
    SafeLog(const SafeLog&) = delete;
    SafeLog& operator=(const SafeLog&) = delete;
    
    ~SafeLog();

    // The Accessor locks the mutex on creation and unlocks on destruction (RAII)
    struct Accessor {
        std::lock_guard<std::mutex> lock;
        FILE* file;
        bool is_open;
    };

    Accessor access();

    bool open(std::string_view path);
    void close();
    long get_size_kb();
};

// Global instance for resmom
extern SafeLog g_job_log;

#endif