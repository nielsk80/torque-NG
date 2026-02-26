# src/lib/Liblog/CMakeLists.txt

# 1. Identify the source files
set(LIBLOG_SOURCES
    chk_file_sec.cpp
    log_event.cpp
    pbs_log.cpp
    pbs_messages.cpp
    setup_env.cpp
    safe_log.cpp
)

# 2. Create the library target
# We'll stick with torque_log or resmom_log (it's a library, not the bin)
add_library(torque_log STATIC ${LIBLOG_SOURCES})

# 3. Set C++ Standard (Required for std::string and ostringstream logic)
target_compile_features(torque_log PRIVATE cxx_std_17)

# 4. Apply 64-bit Large File Support
# This is crucial so that 'off_t' in your refactored code is 64-bit
target_compile_definitions(torque_log PRIVATE _FILE_OFFSET_BITS=64)

# 5. Set the include directories
target_include_directories(torque_log PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src/include
)

# 6. Standard compiler flags
target_compile_options(torque_log PRIVATE -Wall -Wextra)