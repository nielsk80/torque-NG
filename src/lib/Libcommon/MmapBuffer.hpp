/*
 * torque-NG: Next Generation Resource Manager
 *
 * Copyright (c) 2026 Kenneth Nielson.
 * Licensed under the OpenPBS v2.3 Software License.
 */

#pragma once

#include <string>
#include <system_error>
#include <cerrno>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

namespace torque_ng {

class MmapBuffer {
public:
    MmapBuffer(const std::string& filename, size_t size) 
        : filename_(filename), size_(size) {
        
        // 1. Open (or create) the file
        fd_ = open(filename_.c_str(), O_RDWR | O_CREAT, 0644);
        if (fd_ == -1) {
            throw std::system_error(errno, std::generic_category(), "Failed to open state file");
        }

        // 2. Ensure the file is the requested size
        // This is critical for SoA arrays to have enough contiguous space
        if (ftruncate(fd_, size_) == -1) {
            close(fd_);
            throw std::system_error(errno, std::generic_category(), "Failed to size state file");
        }

        // 3. Map the file into memory
        data_ = mmap(nullptr, size_, PROT_READ | PROT_WRITE, MAP_SHARED, fd_, 0);
        if (data_ == MAP_FAILED) {
            close(fd_);
            throw std::system_error(errno, std::generic_category(), "Failed to mmap state file");
        }
    }

    // RAII Cleanup: Ensure data is flushed to disk before closing
    ~MmapBuffer() {
        if (data_ != MAP_FAILED) {
            msync(data_, size_, MS_SYNC); // Hard flush to persistence
            munmap(data_, size_);
        }
        if (fd_ != -1) {
            close(fd_);
        }
    }

    // Prevent accidental copying (don't want two objects managing one mmap)
    MmapBuffer(const MmapBuffer&) = delete;
    MmapBuffer& operator=(const MmapBuffer&) = delete;

    // Accessors
    void* get_ptr() const { return data_; }
    size_t size() const { return size_; }

    // Manual sync for high-frequency updates
    void flush() {
        msync(data_, size_, MS_ASYNC);
    }

private:
    std::string filename_;
    size_t size_;
    int fd_ = -1;
    void* data_ = MAP_FAILED;
};

} // namespace torque_ng
