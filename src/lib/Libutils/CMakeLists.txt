# torque-NG: Libutils Build Configuration
# Copyright (c) 2026 Kenneth Nielson.

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
    ResourceParser.cpp
)

# 3. Create Library
# Standardized to Libutils
add_library(Libutils STATIC ${LIBUTILS_SOURCES})

# 4. Modern Header Management
target_include_directories(Libutils 
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}
    PUBLIC  ${PROJECT_SOURCE_DIR}/src/include      # Allows finding allocation.hpp
    PUBLIC  ${PROJECT_SOURCE_DIR}/src/lib/Liblog   # Allows finding pbs_log.h
    PUBLIC  ${CMAKE_BINARY_DIR}/src                # For generated protos
    PUBLIC  ${CMAKE_BINARY_DIR}/src/proto                # For generated protos
    PRIVATE ${HWLOC_INCLUDE_DIRS}
)

# 5. Dependency Linking
target_link_libraries(Libutils 
    PUBLIC  Liblog 
    PUBLIC  ${HWLOC_LIBRARIES}
    PUBLIC  Threads::Threads
)

# 6. Compiler Features
target_compile_features(Libutils PUBLIC cxx_std_20)
