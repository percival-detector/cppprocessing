
set(CMAKE_INCLUDE_CURRENT_DIR on)

# FIXME: we should not depend on anything parallel in here I think...
include_directories(${HDF5_INCLUDE_DIRS} ../percival_parallel/)

# Add the AVX flag
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx" )

# Build list of test source files from current dir
file(GLOB PCVL_ALGORITHM_SOURCE *.cpp)

# Add project source files to library
add_library(pcvl-algorithms ${PCVL_ALGORITHM_SOURCE})

# Define libraries to link against
target_link_libraries(pcvl-algorithms ${HDF5_LIBRARIES})

