# torque-NG: Libproto Build Configuration
# Copyright (c) 2026 Kenneth Nielson.
# SPDX-License-Identifier: OpenPBS-2.3

# torque-NG: Libproto Logic and Vertical Slice Tests
cmake_minimum_required(VERSION 3.10)
project(torque-NG-Libproto LANGUAGES CXX)

# 1. Define the Logic Library
# This contains your hand-written code like Job.cpp
add_library(torque_proto_logic STATIC 
    Job.cpp
    job_manager.cpp
)

# 2. Header Management for Logic
target_include_directories(torque_proto_logic PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/src/include
)

# 3. Define the Test Binaries
add_executable(server_test server_test.cpp)
add_executable(client_test client_test.cpp)

# 4. Target-Specific Includes for Networking
# Allows tests to find NetworkManager.hpp from Libnet
target_include_directories(server_test PRIVATE ${PROJECT_SOURCE_DIR}/src/lib/Libnet)
target_include_directories(client_test PRIVATE ${PROJECT_SOURCE_DIR}/src/lib/Libnet)

# 5. Dependency Linking
# This connects the logic to the generated protobuf code
target_link_libraries(torque_proto_logic PUBLIC torque_proto)

# Connect the tests to the logic and the network transport
target_link_libraries(server_test PRIVATE torque_proto_logic net_ng)
target_link_libraries(client_test PRIVATE torque_proto_logic net_ng)