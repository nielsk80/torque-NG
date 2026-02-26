#include <safe_log.hpp>
#include <iostream>
#include <string>
#include <string_view>

// Instantiate the global object
SafeLog g_job_log;

SafeLog::~SafeLog() {
    close();
}

/**
 * access - Acquires the mutex and returns an Accessor object.
 * The lock is released automatically when the Accessor goes out of scope.
 */
SafeLog::Accessor SafeLog::access() {
    return {std::lock_guard<std::mutex>(m_mutex), m_logfile, m_logfile != nullptr};
}

/**
 * open - Opens the log file in append mode.
 * Converts string_view to a temporary std::string to ensure null-termination for fopen.
 */
bool SafeLog::open(std::string_view path) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_logfile) fclose(m_logfile);
    
    m_logfile = fopen(path.data(), "a");
    return m_logfile != nullptr;
}

void SafeLog::close() {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_logfile) {
        fclose(m_logfile);
        m_logfile = nullptr;
    }
}

/**
 * get_size_kb - Thread-safe check of the log file size.
 * Returns the size in Kilobytes (KB).
 */
long SafeLog::get_size_kb() {
    auto log = access();
    if (!log.is_open) return 0;

    struct stat st;
    // fileno() extracts the descriptor from the FILE stream
    if (fstat(fileno(log.file), &st) == 0) {
        return static_cast<long>(st.st_size / 1024);
    }
    return 0;
}