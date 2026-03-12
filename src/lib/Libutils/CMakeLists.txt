# torque-NG: Libutils Build Configuration
cmake_minimum_required(VERSION 3.12)
project(torque-NG-Libutils LANGUAGES CXX)

# 1. External Dependencies
find_package(PkgConfig REQUIRED)
pkg_check_modules(HWLOC REQUIRED hwloc)
find_package(Threads REQUIRED)

# 2. Source Files
set(LIBUTILS_SOURCES
    machine.cpp
    topology.cpp
    factory.cpp
    cgroup_manager.cpp
    strategy.cpp
    performance_strategy.cpp
    authorized_hosts.cpp
    allocation.cpp
    user_context.cpp
    jsoncpp.cpp
)

# 3. Create Library
add_library(utils_ng STATIC ${LIBUTILS_SOURCES})

# 4. Modern Header Management
# We use PUBLIC so that Libproto/MOM can see these headers automatically
target_include_directories(utils_ng 
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/../../include
    PRIVATE ${HWLOC_INCLUDE_DIRS}
)

# 5. Dependency Linking
# Linking log_ng as PUBLIC means anyone using utils_ng also gets the logger
target_link_libraries(utils_ng 
    PUBLIC  log_ng 
    PUBLIC  ${HWLOC_LIBRARIES}
    PUBLIC  Threads::Threads
)

# GCC 8 filesystem support
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)
    target_link_libraries(utils_ng PUBLIC stdc++fs)
endif()