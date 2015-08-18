
set(CMAKE_INCLUDE_CURRENT_DIR on)
ADD_DEFINITIONS(-DBOOST_TEST_DYN_LINK)

include_directories(${Boost_INCLUDE_DIRS} ${HDF5_INCLUDE_DIRS} )

# Build list of test source files from current dir
file(GLOB TEST_SOURCES *.cpp)

# Build a list of algorithm source files from the src dir.
# We (re)build the algorithms directly into the profiling application so that we
# can get the correct compiler/linker flags for debugging.
file(GLOB PCVL_ALGORITHM_SOURCE ../../src/*.cpp)

# Add the debugging compiler flags.
set( CMAKE_CXX_FLAGS "-O0 -g3 -pg -DDEBUG" )

# Add test and project source files to executable
add_executable(pcvl-unittest
               ${PCVL_ALGORITHM_SOURCE}
               ${TEST_SOURCES})

# Define libraries to link against
target_link_libraries(pcvl-unittest 
                      pcvl-h5loader
                      ${Boost_LIBRARIES})

