# --- resmom Component Build ---

# 1. Find Hardware Locality (Essential for your 20-core hybrid setup)
find_package(PkgConfig REQUIRED)
pkg_check_modules(HWLOC REQUIRED hwloc)

# 2. Find Security & Communication Dependencies
find_package(OpenSSL REQUIRED)
find_package(LibXml2 REQUIRED)
find_package(Threads REQUIRED)

# 3. Source Files
# We include the legacy C files and your new C++ Modernization files
set(RESMOM_SOURCES
    mom_main.c
    mom_server.c
    requests.c
    start_exec.c
    # Add your new C++ files here as you create them:
    # ResourceManager.cpp
    # HardwareTopology.cpp
)

# 4. Define the Executable
add_executable(pbs_mom ${RESMOM_SOURCES})

# 5. Include Directories
# We point to the root include folder, the local directory, and dependency headers
target_include_directories(pbs_mom PRIVATE 
    ${CMAKE_SOURCE_DIR}/src/include
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${HWLOC_INCLUDE_DIRS}
    ${OPENSSL_INCLUDE_DIR}
    ${LIBXML2_INCLUDE_DIR}
)

# 6. Preprocessor Definitions (The "Config" Bridge)
# These flags tell the legacy code how to behave without the old Autotools config.h
target_compile_definitions(pbs_mom PRIVATE 
    PBS_MOM 
    _GNU_SOURCE 
    NUMA_SUPPORT
    TORQUE_HOME="/var/spool/torque"
)

# 7. Linking
# We link the logger we built in the previous step plus the system libs
target_link_libraries(pbs_mom 
    torque_log      # Our modernized C++ log wrapper
    ${HWLOC_LIBRARIES}
    OpenSSL::SSL 
    OpenSSL::Crypto
    LibXml2::LibXml2
    ${CMAKE_THREAD_LIBS_INIT}
    m               # Math library
)

# 8. Modernization Safety (Optional but recommended for your exercise)
# This helps find memory leaks in the legacy C code immediately
target_compile_options(pbs_mom PRIVATE -fsanitize=address -g)
target_link_options(pbs_mom PRIVATE -fsanitize=address)