# tests/lib/Libproto/CMakeLists.txt

# 1. Create the test executable
add_executable(test_dispatcher test_dispatcher.cpp)
add_executable(test_obituary test_obituary.cpp)
add_executable(test_job_status test_job_status.cpp)
add_executable(test_delete test_delete.cpp)

# 2. Add the missing Include Directories for the Delete test
# This allows the compiler to find Dispatcher.hpp, Job.hpp, etc.
target_include_directories(test_delete PRIVATE 
    ${PROJECT_SOURCE_DIR}/src/lib/Libproto     # Path to Dispatcher.hpp
    ${PROJECT_SOURCE_DIR}/src/proto            # Path to .proto files
    ${PROJECT_BINARY_DIR}/src/proto            # Path to generated .pb.h files
)

# 3. Repeat the include logic for your other tests if they use those headers
target_include_directories(test_job_status PRIVATE 
    ${PROJECT_SOURCE_DIR}/src/proto
    ${PROJECT_BINARY_DIR}/src/proto
)

# 4. Link against the protocol library and logic
# Since you're using GTest, ensure you link that as well
target_link_libraries(test_delete PRIVATE 
    torque_proto 
    ${Protobuf_LIBRARIES}
    GTest::GTest 
    GTest::Main
)

target_link_libraries(test_job_status PRIVATE 
    torque_proto 
    ${Protobuf_LIBRARIES}
)

target_link_libraries(test_dispatcher PRIVATE torque_proto)
target_link_libraries(test_obituary PRIVATE torque_proto)

# 5. CTest support
add_test(NAME Libproto_Dispatcher_Test COMMAND test_dispatcher)
add_test(NAME Libproto_Obituary_Test COMMAND test_obituary)
add_test(NAME Libproto_JobStatus_Test COMMAND test_job_status)
add_test(NAME Libproto_Delete_Test COMMAND test_delete)