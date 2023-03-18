cmake_minimum_required (VERSION 2.8)

project(test_hash)

SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")

include_directories(
    ${PROJECT_ROOT_DIR}/platform/common/inc
    ${PROJECT_ROOT_DIR}/library/credentials/inc
	${PROJECT_ROOT_DIR}/algorithms/symmCrypto/inc
	${PROJECT_ROOT_DIR}/algorithms/hash/inc
)

add_executable(test_hash2 test_hash.cpp)
add_executable(test_hmac2 test_hmac.cpp)

target_link_libraries(test_hash2 algorithms library platform)
target_link_libraries(test_hmac2 algorithms library platform)