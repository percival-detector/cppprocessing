
set(CMAKE_INCLUDE_CURRENT_DIR on)

include_directories(${HDF5_INCLUDE_DIRS} ${TBB_INCLUDE_DIRS} ../percival_parallel/)

# Build list of test source files from current dir
file(GLOB PCVL_PROFILING_SOURCE *.cpp)

# Build a list of algorithm source files from the src dir.
# We (re)build the algorithms directly into the profiling application so that we
# can get the correct compiler/linker flags for profiling.
file(GLOB PCVL_ALGORITHM_SOURCE ../src/*.cpp)

# Add the profiling compiler flags.
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg" )
# Somehow the profiling flags also make it to the linker flags automatically
# so the following line is not necessary...
# set( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg" )

message (STATUS "Profiling sources:      " ${PCVL_PROFILING_SOURCE})
message (STATUS "Algorithm sources:      " ${PCVL_ALGORITHM_SOURCE})
message (STATUS "CMAKE_CXX_FLAGS:        " ${CMAKE_CXX_FLAGS})
message (STATUS "CMAKE_EXE_LINKER_FLAGS: " ${CMAKE_EXE_LINKER_FLAGS})

# Add application source files to executable
add_executable(pcvl-profiling 
               ${PCVL_PROFILING_SOURCE}
               ${PCVL_ALGORITHM_SOURCE})

# Define libraries to link against
target_link_libraries(pcvl-profiling pcvl-h5loader pcvl-tbb-algorithms)

