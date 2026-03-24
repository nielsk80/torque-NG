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
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# 5. Dependency Linking
# This connects the logic to the generated protobuf code
target_link_libraries(torque_proto_logic PUBLIC torque_proto)

