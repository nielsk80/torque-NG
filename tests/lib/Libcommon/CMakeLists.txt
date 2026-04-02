# torque-NG: Libcommon Unit Tests
# Copyright (c) 2026 Kenneth Nielson.
# Portions Copyright (c) 1999-2000 Veridian Information Solutions, Inc.
# Licensed under the OpenPBS v2.3 Software License.

cmake_minimum_required(VERSION 3.10)

# 1. Find Test Dependencies
find_package(GTest REQUIRED)

# 2. Define the Single Test Executable
# This matches your test_managers.cpp file
add_executable(manager_test test_managers.cpp)

# 3. Define Include Paths
# Pointing to the source and binary trees for headers and protos
target_include_directories(manager_test PRIVATE 
    ${PROJECT_SOURCE_DIR}/src/lib/Libcommon
    ${PROJECT_SOURCE_DIR}/src/include
    ${CMAKE_BINARY_DIR}/src
)

# 4. Compiler Configuration
target_compile_features(manager_test PRIVATE cxx_std_20)

# 5. Linkage Configuration
# Linking against the renamed Lib* targets
target_link_libraries(manager_test PRIVATE 
    Libcommon
    torque_proto_logic
    Libproto
    Liblog
    GTest::gtest
    GTest::gtest_main
    Threads::Threads
)

# 6. Register with CTest
add_test(NAME Libcommon_ManagerTest COMMAND manager_test)
