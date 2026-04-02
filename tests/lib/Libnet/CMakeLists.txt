# tests/lib/Libnet/CMakeLists.txt
set(CMAKE_CXX_STANDARD 20)

# 1. Define the test executable
add_executable(test_libnet
    test_async_conn.cpp
)

# 2. Link against GTest and the torque-NG libraries
target_link_libraries(test_libnet
    PRIVATE
    GTest::gtest_main
    Libnet
    Libproto
    Liblog
    Libcommon
    ${Protobuf_LIBRARIES}
    Threads::Threads
)

# 3. FIX: Apply include directories to the CORRECT target (test_libnet)
# We include 'src/lib' so that #include "Libnet/AsyncConnection.hpp" works.
target_include_directories(test_libnet PRIVATE 
    ${CMAKE_SOURCE_DIR}/src/lib
    ${CMAKE_SOURCE_DIR}/src           # For hand-written headers like #include "Libnet/..."
    ${CMAKE_BINARY_DIR}/src           # FIX: Allows #include "proto/tm_messages.pb.h"
)

add_dependencies(test_libnet Libproto)

# 4. Register the test with CTest
add_test(NAME LibnetTests COMMAND test_libnet)