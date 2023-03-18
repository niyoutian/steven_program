cmake_minimum_required (VERSION 2.8)

project(test_avlTree)

SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")

include_directories(
    ${PROJECT_ROOT_DIR}/platform/common/inc
	${PROJECT_ROOT_DIR}/algorithms/symmCrypto/inc
    ${PROJECT_ROOT_DIR}/algorithms/avlTree/inc
)

add_executable(test_avlTree test_avlTree.cpp)

target_link_libraries(test_avlTree algorithms library platform)