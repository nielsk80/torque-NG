# --- 1. Generate C++ Files from Proto ---
# This command finds the .proto file and generates torque_ng.pb.cc and torque_ng.pb.h
# into the CMAKE_CURRENT_BINARY_DIR (build/src/proto).
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS torque_ng.proto)

# --- 2. Create the Protocol Library ---
# We bundle the generated source and headers into a static library.
# This allows other libraries like Libdis and Libnet to link against it easily.
add_library(torque_proto STATIC ${PROTO_SRCS} ${PROTO_HDRS})

# --- 3. Link Dependencies ---
# Ensure the library has access to the Protobuf headers and runtime.
target_link_libraries(torque_proto PUBLIC protobuf::libprotobuf)

# --- 4. Export Headers for Other Targets ---
# This ensures that any target linking to 'torque_proto' automatically
# knows where to find the generated .pb.h files.
target_include_directories(torque_proto PUBLIC ${CMAKE_CURRENT_BINARY_DIR})