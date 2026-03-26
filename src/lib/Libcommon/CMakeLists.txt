find_package(Threads REQUIRED)

# We do NOT call protobuf_generate_cpp here because 
# torque_proto_logic (in src/lib/Libproto) is already doing it.

add_library(torque_common 
    NodeStatusManager.cpp 
    JobQueueManager.cpp 
    JobIndexManager.cpp
)

# Include directories so the Managers can find torque_ng.pb.h
target_include_directories(torque_common PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    # This points to where Libproto's generated headers are located
    ${CMAKE_BINARY_DIR}/src/lib/Libproto
)

# Link against the library that already contains the Compiled Protos
target_link_libraries(torque_common PUBLIC 
    torque_proto_logic
    Threads::Threads
    ${Protobuf_LIBRARIES}
)