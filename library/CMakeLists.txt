project(library)

FILE(GLOB src_files
    "credentials/src/*.cpp"
    "encoder/src/*.cpp"
    "xfrm/src/*.cpp"
)

#list(REMOVE_ITEM src_files ${PROJECT_SOURCE_DIR}/net/src/mxEngineCurl.cpp)

SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")




include_directories(
    ${PROJECT_ROOT_DIR}/platform/common/inc
    ${PROJECT_SOURCE_DIR}/credentials/inc
    ${PROJECT_SOURCE_DIR}/encoder/inc
    ${PROJECT_SOURCE_DIR}/xfrm/inc
)

#target_link_libraries(${PROJECT_NAME} pthread)
add_library(${PROJECT_NAME} SHARED ${src_files})
target_link_libraries(${PROJECT_NAME}  ssl crypto)

#install(FILES common/inc/mxStatus.h DESTINATION ${PROJECT_BINARY_DIR}/../include)
