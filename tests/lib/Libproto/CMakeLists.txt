# tests/lib/Libproto/CMakeLists.txt

# Create the test executable
add_executable(test_dispatcher test_dispatcher.cpp)
add_executable(test_obituary test_obituary.cpp)

# Link against the protocol library
# CMake handles the header paths automatically if you used 'PUBLIC' in src/proto
target_link_libraries(test_dispatcher PRIVATE torque_proto)
target_link_libraries(test_obituary PRIVATE torque_proto)
# Optional: Add to CTest for 'make test' support
add_test(NAME Libproto_Dispatcher_Test COMMAND test_dispatcher)
add_test(NAME Libproto_Obituary_Test COMMAND test_obituary)
