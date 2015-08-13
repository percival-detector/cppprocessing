
set(CMAKE_INCLUDE_CURRENT_DIR on)

include_directories(${HDF5_INCLUDE_DIRS})

# Build list of test source files from current dir
file(GLOB PCVL_LOADER_SOURCE *.cpp)
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" )

# Add project source files to library
add_library(pcvl-h5loader ${PCVL_LOADER_SOURCE})

# Define libraries to link against
target_link_libraries(pcvl-h5loader ${HDF5_LIBRARIES})

