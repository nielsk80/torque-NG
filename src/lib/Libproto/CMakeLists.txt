find_package(Protobuf REQUIRED)

# This command tells CMake to find your .proto file and run protoc
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ../../proto/job_status.proto)

add_library(torque_proto STATIC 
    ${PROTO_SRCS} 
    ${PROTO_HDRS}
    proto_bridge.cpp
)

target_include_directories(torque_proto PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(torque_proto PUBLIC protobuf::libprotobuf)