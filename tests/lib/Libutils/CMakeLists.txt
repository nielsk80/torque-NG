# torque-NG: Libutils Unit Tests
# Copyright (c) 2026 Kenneth Nielson.
# Licensed under the OpenPBS v2.3 Software License.

# 1. Define the test executable
add_executable(test_resource_parser TestResourceParser.cpp)

# 2. Include directories
target_include_directories(test_resource_parser PRIVATE 
    ${CMAKE_SOURCE_DIR}/src/lib/Libutils
    ${PROJECT_SOURCE_DIR}/src/include
    ${CMAKE_BINARY_DIR}/src             # For #include "proto/torque_ng.pb.h"
)

# 3. Linkage Configuration
# Standardized to use the new Lib* target names
target_link_libraries(test_resource_parser PRIVATE 
    Libutils
    Libproto
    Liblog
    ${Protobuf_LIBRARIES}
    Threads::Threads
)

# 4. Standard and Registration
target_compile_features(test_resource_parser PRIVATE cxx_std_20)
add_test(NAME Libutils_ResourceParserTest COMMAND test_resource_parser)
