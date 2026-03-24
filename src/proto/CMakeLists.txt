# torque-NG: Libproto Build Configuration
# Copyright (c) 2026 Kenneth Nielson.
# SPDX-License-Identifier: OpenPBS-2.3

cmake_minimum_required(VERSION 3.10)

# 1. Generate C++ Files from our updated unified contract
# Removed job_status.proto and torque_messages.proto to prevent collisions
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS 
    torque_ng.proto
)

# 2. Create the Protocol Library
add_library(torque_proto STATIC ${PROTO_SRCS} ${PROTO_HDRS})

# 3. Compiler Requirements
target_compile_features(torque_proto PUBLIC cxx_std_17)
set_target_properties(torque_proto PROPERTIES POSITION_INDEPENDENT_CODE ON)

# 4. Dependency Linking
target_link_libraries(torque_proto PUBLIC protobuf::libprotobuf)

# 5. Header Export
# This allows other targets to find torque_ng.pb.h and Dispatcher.hpp
target_include_directories(torque_proto PUBLIC 
   ${CMAKE_CURRENT_BINARY_DIR}  # Generated .pb.h files
   ${CMAKE_CURRENT_SOURCE_DIR}  # Hand-written headers (Dispatcher.hpp)
)