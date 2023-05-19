cmake_minimum_required (VERSION 3.0)

project(steven_program)

SET(CMAKE_BUILD_TYPE "Debug")
# SET(CMAKE_C_FLAGS_DEBUG "$ENV{CFLAGS} -O0 -Wall -g2 -ggdb")
# SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
SET(CMAKE_C_FLAGS_DEBUG "$ENV{CFLAGS} -g -O0 -Wall")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -g -O0 -Wall")

message("CMAKE_C_FLAGS_DEBUG=${CMAKE_C_FLAGS_DEBUG}")

SET(PROJECT_ROOT_DIR ${PROJECT_SOURCE_DIR})

set(LIBRARY_OUTPUT_PATH  ${PROJECT_BINARY_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

add_subdirectory(platform)
add_subdirectory(algorithms)
add_subdirectory(library)
add_subdirectory(test)
add_subdirectory(app)

