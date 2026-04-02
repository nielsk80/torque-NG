# src/resmom/CMakeLists.txt
# torque-NG: Resource Manager MOM (Node Daemon)
#
# Copyright (c) 2026 Kenneth Nielson.
# All rights reserved.

cmake_minimum_required(VERSION 3.10)

# 1. External Dependencies
find_package(Threads REQUIRED)
find_package(Protobuf REQUIRED)
# Boost is required for Asio and Coroutine support
find_package(Boost REQUIRED COMPONENTS system)

# 2. Define the MOM Executable
add_executable(mom_ng
    mom_ng_main.cpp
    # Add other modernized files as they are finalized
    # ProcessManager.cpp
    # JobOutputHandler.cpp
)

# 3. Compiler Configuration
target_compile_features(mom_ng PRIVATE cxx_std_20)

# GCC 10/11 often requires -fcoroutines to enable the feature for C++20
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(mom_ng PRIVATE -fcoroutines)
endif()

# 4. Header Management
target_include_directories(mom_ng PRIVATE 
    ${PROJECT_SOURCE_DIR}/src           # For #include "resmom/..."
    ${PROJECT_SOURCE_DIR}/src/include   # For #include "log.h"
    ${CMAKE_BINARY_DIR}/src             # For #include "proto/tm_messages.pb.h"
)

# 5. Dependency Linking
# We link to the standardized "Lib" targets we defined in the other CMake files
target_link_libraries(mom_ng
    PRIVATE
    Libproto        # Generated Protobuf classes
    Libcommon       # Node and Job managers
    Liblog          # Modern logging and security checks
    Libnet          # Asynchronous networking
    Libutils        # Hardware topology and cgroups
    ${Protobuf_LIBRARIES}
    Boost::boost
    Threads::Threads
)

# 6. Installation (Standard Spanish Fork /var/spool/torque paths)
install(TARGETS mom_ng DESTINATION sbin)