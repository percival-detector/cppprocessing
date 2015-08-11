
set(CMAKE_INCLUDE_CURRENT_DIR on)

include_directories(${HDF5_INCLUDE_DIRS})

# Build list of test source files from current dir
file(GLOB PCVL_PROFILING_SOURCE *.cpp)

# Add application source files to executable
add_executable(pcvl-profiling ${PCVL_PROFILING_SOURCE})

# Define libraries to link against
target_link_libraries(pcvl-profiling pcvl-algorithms)

