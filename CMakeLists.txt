# torque-NG: Libproto Build Configuration
# Copyright (c) 2026 Kenneth Nielson.
# SPDX-License-Identifier: OpenPBS-2.3

# torque-NG: Root Build Configuration
cmake_minimum_required(VERSION 3.10)
project(torque-NG LANGUAGES CXX)

# Set C++ Standard for the whole project
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Find Protobuf globally
find_package(Protobuf REQUIRED)

# 2. Add Subdirectories in Dependency Order
# We must generate the raw proto library before the logic library can use it
add_subdirectory(src/proto)      
add_subdirectory(src/lib/Liblog)
add_subdirectory(src/lib/Libutils)
add_subdirectory(src/lib/Libnet)
add_subdirectory(src/lib/Libproto)