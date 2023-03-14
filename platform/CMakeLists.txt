cmake_minimum_required (VERSION 2.8)

project(platform)

FILE(GLOB src_files
    "common/src/*.cpp"
    "net/src/*.cpp"
    "process/src/*.cpp"
)

list(REMOVE_ITEM src_files ${PROJECT_SOURCE_DIR}/net/src/mxEngineCurl.cpp)
message("=== src_files is "${src_files})

SET(CMAKE_BUILD_TYPE "Debug")




include_directories(
    ${PROJECT_SOURCE_DIR}/common/inc
    ${PROJECT_SOURCE_DIR}/net/inc
    ${PROJECT_SOURCE_DIR}/process/inc
)
message("=== PROJECT_SOURCE_DIR is "${PROJECT_SOURCE_DIR})
message("=== PROJECT_BINARY_DIR is "${PROJECT_BINARY_DIR})
message("=== PROJECT_BINARY_DIR is "${PROJECT_BINARY_DIR})

#target_link_libraries(${PROJECT_NAME} pthread)
add_library(${PROJECT_NAME} SHARED ${src_files})

install(FILES common/inc/mxStatus.h DESTINATION ${PROJECT_BINARY_DIR}/../include)
