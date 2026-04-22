# torque-NG: Liblog Build Configuration
cmake_minimum_required(VERSION 3.10)
project(torque-NG-Liblog LANGUAGES CXX)

# 1. Define the source files
set(LIBLOG_SOURCES
    safe_log.cpp
    chk_file_sec.cpp
    pbs_log.cpp
    log_event.cpp
    setup_env.cpp
    pbs_messages.cpp
    TorqueErrors.cpp
)

# 2. Create the static library
add_library(Liblog STATIC ${LIBLOG_SOURCES})

# 3. Modern Header Management (The "Senior" Way)
# PUBLIC: Anyone who links to Liblog needs these headers
# PRIVATE: Only Liblog needs these internal includes
target_include_directories(Liblog 
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}
    PUBLIC  ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)

# 4. Target-Specific Linkage
# This ensures pthread and filesystem support follow the library
target_link_libraries(Liblog PUBLIC Threads::Threads)

# GCC 8 filesystem support (Compatibility for Ubuntu 18.04/20.04)
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9.0)
    target_link_libraries(Liblog PUBLIC stdc++fs)
endif()