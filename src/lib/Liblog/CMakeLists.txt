cmake_minimum_required(VERSION 3.10)

# torque-NG: Next Generation Resource Manager
project(torque-NG-Liblog VERSION 1.0.0 LANGUAGES CXX)

# Default to Debug build if no build type is specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
endif()

# Set compiler-specific flags for Debugging
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Configuring for Debug build...")
    add_compile_options(-g -O0 -Wall -Wextra)
else()
    message(STATUS "Configuring for Release build...")
    add_compile_options(-O3)
endif()

# Use C++17 for std::filesystem and std::scoped_lock
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add include directory (assuming headers are in the same folder or 'include')
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../include)

# Define the source files for the library
set(LIBLOG_SOURCES
    safe_log.cpp
    chk_file_sec.cpp
    pbs_log.cpp
    log_event.cpp
    setup_env.cpp
    pbs_messages.cpp
)

# Create the static library liblog_ng.a
add_library(log_ng STATIC ${LIBLOG_SOURCES})

# Link necessary system libraries
# Networking functions need no extra libs on modern Linux, 
# but filesystem might need 'stdc++fs' on older GCC versions.
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)
    target_link_libraries(log_ng PUBLIC stdc++fs)
endif()

# Threading support for std::mutex
find_package(Threads REQUIRED)
target_link_libraries(log_ng PUBLIC Threads::Threads)

# --- Optional: Add a small test executable to verify compilation ---
add_executable(log_test test/main.cpp)
target_link_libraries(log_test log_ng)