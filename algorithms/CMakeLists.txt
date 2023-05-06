project(algorithms)

FILE(GLOB src_files
    "avlTree/src/*.cpp"
    "hash/src/*.cpp"
    "symmCrypto/src/*.cpp"
)

#list(REMOVE_ITEM src_files ${PROJECT_SOURCE_DIR}/net/src/mxEngineCurl.cpp)

SET(CMAKE_BUILD_TYPE "Debug")




include_directories(
    ${PROJECT_ROOT_DIR}/platform/common/inc
    ${PROJECT_SOURCE_DIR}/avlTree/inc
    ${PROJECT_SOURCE_DIR}/hash/inc
    ${PROJECT_SOURCE_DIR}/symmCrypto/inc
)

#target_link_libraries(${PROJECT_NAME} pthread)
add_library(${PROJECT_NAME} SHARED ${src_files})
