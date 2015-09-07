
set(CMAKE_INCLUDE_CURRENT_DIR on)

include_directories(${HDF5_INCLUDE_DIRS} ${TBB_INCLUDE_DIRS} ../percival_parallel/)

# Add the AVX flag
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx" )

# Build list of test source files from current dir
file(GLOB PCVL_PARALLEL_TEST_SOURCE *.cpp)

# Add application source files to executable
add_executable(pcvl-tbb 
               ${PCVL_PARALLEL_TEST_SOURCE})

# Define libraries to link against
target_link_libraries(pcvl-tbb pcvl-h5loader pcvl-tbb-algorithms pcvl-algorithms)

