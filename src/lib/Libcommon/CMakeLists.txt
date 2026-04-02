# torque-NG: Common Management Logic
# Copyright (c) 2026 Kenneth Nielson.
# Portions Copyright (c) 1999-2000 Veridian Information Solutions, Inc.
# Licensed under the OpenPBS v2.3 Software License.

cmake_minimum_required(VERSION 3.10)

# 1. External Dependencies
find_package(Threads REQUIRED)

# 2. Define the Library
# We list the sources directly to ensure the build is explicit.
add_library(Libcommon STATIC 
    NodeStatusManager.cpp 
    JobQueueManager.cpp 
    JobIndexManager.cpp
)

# 3. Header Management
# PUBLIC ensures that mom_ng and tests can find these headers.
target_include_directories(Libcommon PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/src/include
    ${CMAKE_BINARY_DIR}/src             # For #include "proto/tm_messages.pb.h"
)

# 4. Dependency Linking
# Linking to standardized Lib* targets
target_link_libraries(Libcommon PUBLIC 
    Libproto
    torque_proto_logic
    Liblog
    Threads::Threads
    ${Protobuf_LIBRARIES}
)