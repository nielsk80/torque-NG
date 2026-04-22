# torque-NG: Server Subsystem Build Configuration
# Copyright (c) 2026 Kenneth Nielson.
# SPDX-License-Identifier: OpenPBS-2.3

cmake_minimum_required(VERSION 3.15)
project(torque-NG-Server LANGUAGES CXX)

# 1. Executable Definition
# This builds the actual pbs_server_ng daemon
add_executable(pbs_server_ng 
    main.cpp
    ServerListener.cpp
)

# 2. Header Management
# Includes the local directory, the global include (for NetTask.hpp), 
# and the Libnet directory (for AsyncConnection.hpp)
target_include_directories(pbs_server_ng PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/src/include
    ${PROJECT_SOURCE_DIR}/src/lib/libnet
)

# 3. Dependency Linking
# Links against your static networking and logging libraries
target_link_libraries(pbs_server_ng PRIVATE 
    Libnet
    Liblog
    Boost::system  # Required for asio
)

# 4. Compiler Requirements & Coroutine Support
target_compile_features(pbs_server_ng PRIVATE cxx_std_20)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(pbs_server_ng PRIVATE -fcoroutines)
endif()

# 5. Installation (Optional)
# Defines where the daemon will be installed on the system
install(TARGETS pbs_server_ng DESTINATION sbin)