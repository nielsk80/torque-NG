cmake_minimum_required(VERSION 3.12)
project(Libutils CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Dependencies
find_package(PkgConfig REQUIRED)
pkg_check_modules(HWLOC REQUIRED hwloc)

# 2. Source Files (Strictly the modernized set)
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

add_library(utils STATIC ${LIBUTILS_SOURCES})

# 3. Include Paths
target_include_directories(utils PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${HWLOC_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/../Liblog
    ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)

# 4. Linking
target_link_libraries(utils PRIVATE 
    ${HWLOC_LIBRARIES}
    pthread
)

# GCC 8 filesystem support
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)
    target_link_libraries(utils PRIVATE stdc++fs)
endif()