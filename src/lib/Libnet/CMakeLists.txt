# torque-NG: Libnet Build Configuration
# Copyright (c) 2026 Kenneth Nielson.
# SPDX-License-Identifier: OpenPBS-2.3

cmake_minimum_required(VERSION 3.10)
project(torque-NG-Libnet LANGUAGES CXX)

# 1. Define the library
# Standardized to Libnet so mom_ng can find it
add_library(Libnet STATIC 
    NetworkManager.cpp
    # Add AsyncConnection.cpp here if you've created it
)

# 2. Header Management
# We include ${CMAKE_BINARY_DIR}/src so we can use #include "proto/tm_messages.pb.h"
target_include_directories(Libnet PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/src/include
    ${CMAKE_BINARY_DIR}/src
)

# 3. Dependency Linking
# Changed log_ng to Liblog to match your new naming standard
target_link_libraries(Libnet PUBLIC 
    Liblog
    Libproto
)

# 4. Compiler Requirements
target_compile_features(Libnet PUBLIC cxx_std_20)

# Ensure coroutine support propagates to anything using Libnet
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(Libnet INTERFACE -fcoroutines)
endif()