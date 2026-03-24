# torque-NG: Next Generation Resource Manager
#
# Copyright (c) 2026 Kenneth Nielson.
# Portions Copyright (c) 1999-2000 Veridian Information Solutions, Inc.
# Licensed under the OpenPBS v2.3 Software License.

# 1. Create the test executables
add_executable(test_dispatcher test_dispatcher.cpp)
add_executable(test_obituary   test_obituary.cpp)
add_executable(test_job_status test_job_status.cpp)
add_executable(test_delete     test_delete.cpp)
add_executable(test_submit     test_submit.cpp)

# 2. Define shared include paths
set(TEST_INCLUDES 
    ${PROJECT_SOURCE_DIR}/src/lib/Libproto     
    ${PROJECT_SOURCE_DIR}/src/proto            
    ${PROJECT_BINARY_DIR}/src/proto            
    ${PROJECT_SOURCE_DIR}/src/include
)

# Apply includes to all test targets
foreach(test_target test_dispatcher test_obituary test_job_status test_delete test_submit)
    target_include_directories(${test_target} PRIVATE ${TEST_INCLUDES})
endforeach()

# 3. Linkage Configuration
# Using 'torque_proto_logic' to match src/lib/Libproto/CMakeLists.txt
foreach(test_target test_delete test_submit test_job_status test_dispatcher test_obituary)
    target_link_libraries(${test_target} PRIVATE 
        torque_proto_logic
        torque_proto 
        ${Protobuf_LIBRARIES}
        GTest::GTest 
        GTest::Main
    )
endforeach()

target_include_directories(test_job_status PRIVATE 
    ${CMAKE_SOURCE_DIR}/src/lib/Libutils
    ${CMAKE_BINARY_DIR}/src/proto # For torque_ng.pb.h
)

# 4. CTest registration
add_test(NAME Libproto_Dispatcher_Test COMMAND test_dispatcher)
add_test(NAME Libproto_Obituary_Test   COMMAND test_obituary)
add_test(NAME Libproto_JobStatus_Test   COMMAND test_job_status)
add_test(NAME Libproto_Delete_Test     COMMAND test_delete)
add_test(NAME Libproto_Submit_Test     COMMAND test_submit)