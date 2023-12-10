# only for cmake --version >= 3.5.1
cmake_minimum_required(VERSION 3.5.1)

# project name
project(chessengine-cmake)

# creates the variable EXEC and sets it to chessengine
set(EXEC chessengine)

# set the C++14 standard
set(CMAKE_CXX_STANDARD 14)

# I../includes
include_directories(includes)

# puts all .cpp files inside src to the SOURCES variable
file(GLOB SOURCES src/*.cpp)


# Unit tests
find_package(Boost COMPONENTS filesystem system unit_test_framework REQUIRED)
add_subdirectory(test)
# Assign the include directories
include_directories(${Boost_INCLUDE_DIRS})

# Add other dir
#add_subdirectory(<dirname>)


# Build
add_executable(${EXEC} ${SOURCES})
#add_executable(cmake_boost_demo ${SOURCE_FILES})
target_link_libraries(${EXEC} ${Boost_LIBRARIES})
