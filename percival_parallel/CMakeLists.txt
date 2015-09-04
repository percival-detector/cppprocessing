
set(CMAKE_INCLUDE_CURRENT_DIR on)

include_directories( ${HDF5_INCLUDE_DIRS}
                     ${TBB_INCLUDE_DIRS} )

# Add the AVX flag
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx" )

# Build list of test source files from current dir
file(GLOB PCVL_TBB_ALGORITHM_SOURCE *.cpp)

# Add project source files to library
add_library(pcvl-tbb-algorithms ${PCVL_TBB_ALGORITHM_SOURCE})

# Define libraries to link against
target_link_libraries(pcvl-tbb-algorithms ${HDF5_LIBRARIES} ${TBB_LIBRARY})

