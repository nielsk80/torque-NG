# torque-NG: Libproto Build Configuration
# Copyright (c) 2026 Kenneth Nielson.
# SPDX-License-Identifier: OpenPBS-2.3

cmake_minimum_required(VERSION 3.10)

# 1. Generate C++ Files
# Added tm_messages.proto here so protoc generates the missing .pb.h
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS 
    torque_ng.proto
    tm_messages.proto
)

# 2. Create the Protocol Library
# Renamed to Libproto to match your test_libnet dependencies
add_library(Libproto STATIC ${PROTO_SRCS} ${PROTO_HDRS})

# 3. Compiler Requirements
target_compile_features(Libproto PUBLIC cxx_std_20) # Updated to 20 for Coroutine compatibility
set_target_properties(Libproto PROPERTIES POSITION_INDEPENDENT_CODE ON)

# 4. Dependency Linking
target_link_libraries(Libproto PUBLIC protobuf::libprotobuf)

# 5. Header Export
target_include_directories(Libproto PUBLIC 
   ${CMAKE_CURRENT_BINARY_DIR}  # Where the .pb.h files will be created
   ${CMAKE_CURRENT_SOURCE_DIR}  # Where the .proto files live
)