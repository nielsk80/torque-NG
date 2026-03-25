# 1. Define the test executable (Test code only)
add_executable(test_resource_parser TestResourceParser.cpp)

# 2. Include directories
target_include_directories(test_resource_parser PRIVATE 
    ${CMAKE_SOURCE_DIR}/src/lib/Libutils
    ${CMAKE_BINARY_DIR}/src/proto
)

# 3. Link against the library you just showed me
# Use 'utils_ng' as defined in src/lib/Libutils/CMakeLists.txt
target_link_libraries(test_resource_parser PRIVATE 
    utils_ng
    torque_proto
    protobuf::libprotobuf
    Threads::Threads
)

# 4. Standard and Registration
target_compile_features(test_resource_parser PRIVATE cxx_std_17)
add_test(NAME ResourceParserTest COMMAND test_resource_parser)