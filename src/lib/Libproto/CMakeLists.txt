# torque-NG: Libproto Build Configuration
# Copyright (c) 2026 Kenneth Nielson.
# SPDX-License-Identifier: OpenPBS-2.3

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
    ${CMAKE_BINARY_DIR}/src/proto  # Path to generated .pb.h files
)

# 3. Dependency Linking
# FIX: Link to 'Libproto' to match the target defined in src/proto/CMakeLists.txt
target_link_libraries(torque_proto_logic PUBLIC 
    Libproto
    Liblog
    Libcommon
)