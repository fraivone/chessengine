# only for cmake --version >= 3.5.1
cmake_minimum_required(VERSION 3.5.1)

# project name
project(chessengine-cmake)

### Upgraded from 14 - 17 to use "std::string_view"
# set the C++17 standard
set(CMAKE_CXX_STANDARD 17)

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3  -march=native -ftree-vectorize -mfpmath=sse -lstdc++")

# I../includes
include_directories(includes)

# puts all .cpp files inside src to the SOURCES variable
file(GLOB SOURCES src/*.cpp)


# Unit tests
find_package(Boost COMPONENTS filesystem system unit_test_framework REQUIRED)
add_subdirectory(test)
# Assign the include directories
include_directories(${Boost_INCLUDE_DIRS})

# creates the variable EXEC with name of the executable
set(MAINEXEC chessengine)
set(UCIEXEC uci)
# get the all app files you want to build an executable from
file(GLOB MAIN app/main.cxx)
file(GLOB UCI app/uci.cxx)

# add all executables you want to build
add_executable(${MAINEXEC} ${MAIN} ${SOURCES})
add_executable(${UCIEXEC} ${UCI} ${SOURCES})
# add boost test, which only links to the main given the other cmake file
target_link_libraries(${MAINEXEC} ${Boost_LIBRARIES})
