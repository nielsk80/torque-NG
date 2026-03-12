# torque-NG: Libproto Build Configuration
# Copyright (c) 2026 Kenneth Nielson.
# SPDX-License-Identifier: OpenPBS-2.3

# torque-NG: Protobuf Generation and Library
cmake_minimum_required(VERSION 3.10)

# 1. Generate C++ Files from Proto
# Using the filename directly tells CMake to look in the current source dir
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS job_status.proto)

# 2. Create the raw Protocol Library
# This library contains ONLY the generated code
add_library(torque_proto STATIC ${PROTO_SRCS} ${PROTO_HDRS})

# 3. Dependency Linking
# PUBLIC ensures anyone linking to torque_proto gets the protobuf headers
target_link_libraries(torque_proto PUBLIC protobuf::libprotobuf)

# 4. Header Export
# This allows other modules to find the generated .pb.h files
target_include_directories(torque_proto PUBLIC ${CMAKE_CURRENT_BINARY_DIR})