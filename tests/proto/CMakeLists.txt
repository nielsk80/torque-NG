# tests/proto/CMakeLists.txt

# 1. Find the proto file
set(PROTO_FILE "${CMAKE_SOURCE_DIR}/src/proto/torque_ng.proto") 

# 2. Tell CMake to generate the C++ files from the .proto
# This creates two variables: PROTO_SRCS (the .cc) and PROTO_HDRS (the .h)
# Note: These are often generated in the CMAKE_CURRENT_BINARY_DIR
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_FILE}) 

# 3. Define the executable using the generated sources
add_executable(proto_integration_test 
    test_dispatcher.cpp 
    test_serialization.cpp
    ${PROTO_SRCS} 
    ${PROTO_HDRS}
) 

# 4. Define shared include paths
# This ensures the compiler can find both your hand-written headers and generated ones
target_include_directories(proto_integration_test PRIVATE 
    ${CMAKE_CURRENT_BINARY_DIR}          # For generated .pb.h files (torque_ng.pb.h) 
    ${CMAKE_SOURCE_DIR}/src/proto        # For proto-related headers 
    ${CMAKE_SOURCE_DIR}/src/utils        # Path to Dispatcher.hpp 
)

# 5. Link everything
target_link_libraries(proto_integration_test 
    PRIVATE 
    GTest::gtest_main 
    protobuf::libprotobuf
    # torque_libutils  <-- Ensure this is built as a library in src/utils/CMakeLists.txt 
)

# 6. Register the test with CTest
add_test(NAME ProtoIntegration COMMAND proto_integration_test) 