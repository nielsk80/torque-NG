# torque-NG: Libproto Build Configuration
# Copyright (c) 2026 Kenneth Nielson.
# SPDX-License-Identifier: OpenPBS-2.3

cmake_minimum_required(VERSION 3.10)

# 1. Generate C++ Files from our updated "Contract"
# We add torque_messages.proto here. 
# You can list multiple files if you split them later.
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS 
    job_status.proto 
    torque_messages.proto
)

# 2. Create the Protocol Library
add_library(torque_proto STATIC ${PROTO_SRCS} ${PROTO_HDRS})

# 3. Compiler Requirements
# Ensure we are using C++17 as discussed for the rest of torque-NG
target_compile_features(torque_proto PUBLIC cxx_std_17)

# Ensure Position Independent Code for library linking compatibility
set_target_properties(torque_proto PROPERTIES POSITION_INDEPENDENT_CODE ON)

# 4. Dependency Linking
target_link_libraries(torque_proto PUBLIC protobuf::libprotobuf)

# 5. Header Export
# CMAKE_CURRENT_BINARY_DIR is where the .pb.h files are generated
target_include_directories(torque_proto PUBLIC 
   ${CMAKE_CURRENT_BINARY_DIR}  # Path to generated .pb.h files
   ${CMAKE_CURRENT_SOURCE_DIR}  # Path to Dispatcher.hpp and TorqueErrors.hpp 
)
