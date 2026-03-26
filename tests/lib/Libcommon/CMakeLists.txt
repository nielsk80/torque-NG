# 1. Find GTest
find_package(GTest REQUIRED)

# 2. Define the test executable
add_executable(manager_test test_managers.cpp)

# 3. Add Include Paths (The Fix)
target_include_directories(manager_test PRIVATE 
    ${CMAKE_SOURCE_DIR}/src/lib/Libcommon    # Path where MmapBuffer.hpp lives
    ${CMAKE_SOURCE_DIR}/src/proto            # Path for generated .pb.h files
    ${CMAKE_BINARY_DIR}/src/proto # For generated pb.h files
)

# 4. Link Libraries
target_link_libraries(manager_test 
    PRIVATE 
    torque_common
    torque_proto_logic     # Your library containing the manager logic
    GTest::gtest
    GTest::gtest_main      # This provides the 'main()' function
)

# 5. Register with CTest
add_test(NAME ManagerIntegrationTest COMMAND manager_test)