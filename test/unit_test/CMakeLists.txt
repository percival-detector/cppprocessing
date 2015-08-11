
set(CMAKE_INCLUDE_CURRENT_DIR on)
ADD_DEFINITIONS(-DBOOST_TEST_DYN_LINK)

include_directories(${Boost_INCLUDE_DIRS} ${HDF5_INCLUDE_DIRS})

# Build list of test source files from current dir
file(GLOB TEST_SOURCES *.cpp)

# Add test and project source files to executable
add_executable(pcvl-unittest ${TEST_SOURCES})

# Define libraries to link against
target_link_libraries(pcvl-unittest pcvl-algorithms ${Boost_LIBRARIES} ${HDF5_LIBRARIES})

