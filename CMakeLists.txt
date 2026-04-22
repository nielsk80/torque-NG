# torque-NG: Libproto Build Configuration
# Copyright (c) 2026 Kenneth Nielson.
# SPDX-License-Identifier: OpenPBS-2.3

# torque-NG: Root Build Configuration
cmake_minimum_required(VERSION 3.10)
project(torque-NG LANGUAGES CXX)

# Set C++ Standard for the whole project
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Initialize the Ctest infrastructure
enable_testing()

# 1. Find Protobuf globally
find_package(Protobuf REQUIRED)
find_package(GTest REQUIRED)
find_package(Boost 1.83 REQUIRED COMPONENTS system)

include_directories(${GTEST_INCLUDE_DIRS})

# 2. Add Subdirectories in Dependency Order
# We must generate the raw proto library before the logic library can use it
add_subdirectory(src/proto)      
add_subdirectory(src/lib/Liblog)
add_subdirectory(src/lib/Libutils)
add_subdirectory(src/lib/Libnet)
add_subdirectory(src/lib/Libproto)
add_subdirectory(src/lib/Libcommon)
add_subdirectory(src/resmom)
add_subdirectory(src/server)
add_subdirectory(tests/lib/Libproto)
add_subdirectory(tests/proto)
add_subdirectory(tests/lib/Libutils)
add_subdirectory(tests/lib/Libcommon)
add_subdirectory(tests/lib/Libnet)
