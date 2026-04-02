# tests/proto/CMakeLists.txt
# Copyright (c) 2026 Kenneth Nielson.
# Licensed under the OpenPBS v2.3 Software License.

set(CMAKE_CXX_STANDARD 20)

# 1. Define the executable WITHOUT generating new .pb files
add_executable(proto_integration_test 
    test_dispatcher.cpp 
    test_serialization.cpp
) 

# 2. Define shared include paths
# We point to the central BUILD directory where Libproto puts its headers
target_include_directories(proto_integration_test PRIVATE 
    ${CMAKE_BINARY_DIR}/src/proto        # The SINGLE source for .pb.h files
    ${CMAKE_SOURCE_DIR}/src/proto        # For the .proto files themselves
    ${CMAKE_SOURCE_DIR}/src/include      # For your project-wide headers
    ${CMAKE_SOURCE_DIR}/src/lib/Libutils # Path to Dispatcher.hpp
)

# 3. Link everything to the central Libproto
target_link_libraries(proto_integration_test 
    PRIVATE 
    Libproto                             # <--- Use the central target we built
    GTest::gtest_main 
    protobuf::libprotobuf
    Threads::Threads
)

# 4. Register the test with CTest
add_test(NAME ProtoIntegration COMMAND proto_integration_test)