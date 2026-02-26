cmake_minimum_required(VERSION 3.12)
project(TorqueModernization LANGUAGES C CXX)

# --- 1. Global Preprocessor Definitions ---
add_definitions(-D_GNU_SOURCE -DPBS_MOM)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF) # Recommended to ensure portability

include(CheckIncludeFile)
check_include_file("sys/types.h" HAVE_SYS_TYPES_H)
check_include_file("sys/stat.h" HAVE_SYS_STAT_H)
check_include_file("unistd.h" HAVE_UNISTD_H)

# This tells CMake to look for a template file and create the real header
configure_file(
    "${CMAKE_SOURCE_DIR}/pbs_config.h.in"
    "${CMAKE_BINARY_DIR}/src/include/pbs_config.h"
)

# Ensure the build directory's include path is visible
include_directories("${CMAKE_BINARY_DIR}/src/include")

# --- 2. System-wide Header Search ---
include_directories(${CMAKE_SOURCE_DIR}/src/include)

# --- 3. Add Subdirectories in order of dependency ---
# We build the library first, then the MOM daemon
add_subdirectory(src/lib/Liblog)
add_subdirectory(src/lib/Libutils)
add_subdirectory(src/resmom)
