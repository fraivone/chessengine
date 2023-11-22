cmake_minimum_required(VERSION 3.5)

# Project settings
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ".")
set(PROJECT_SER_NAME serialize)

# Dependencies
#find_package(Boost 1.70.0 COMPONENTS system filesystem serialization REQUIRED)
find_package(Boost 1.65.1.0 REQUIRED system serialization)

# Include paths
set(PROJECT_SER_INCLUDES ${PROJECT_INCLUDE_DIR}
        ${PROJECT_SOURCE_DIR}/includes
        ${PROJECT_SOURCE_DIR}/src
        ${Boost_INCLUDE_DIR})

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${PROJECT_SER_INCLUDES})

# Gather the header and source files
file(GLOB SRC_FILES ${PROJECT_SOURCE_DIR}/src/*.cpp)
## exclude main.cpp from the list of srcs
list(REMOVE_ITEM SRC_FILES "/home/francesco/Programs/engine/src/main.cpp")

# Build unit tests
add_executable(${PROJECT_SER_NAME} ${SRC_FILES} ${PROJECT_SOURCE_DIR}/Serialize/ser.cpp)
target_link_libraries(${PROJECT_SER_NAME}  ${Boost_LIBRARIES})
