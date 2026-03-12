# torque-NG: Libnet Build Configuration
cmake_minimum_required(VERSION 3.10)
project(torque-NG-Libnet LANGUAGES CXX)

# 1. Define the library
add_library(net_ng STATIC NetworkManager.cpp)

# 2. Header Management
# PUBLIC ensures that anyone linking to net_ng (like server_test) 
# automatically finds NetworkManager.hpp.
target_include_directories(net_ng 
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}
    PUBLIC  ${PROJECT_SOURCE_DIR}/src/include
)

# 3. Dependency Linking
# Networking often needs logging for connection events
target_link_libraries(net_ng PUBLIC log_ng)