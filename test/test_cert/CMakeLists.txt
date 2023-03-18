cmake_minimum_required (VERSION 2.8)

project(test_cert)

SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")

include_directories(
    ${PROJECT_ROOT_DIR}/platform/common/inc
    ${PROJECT_ROOT_DIR}/library/credentials/inc
	${PROJECT_ROOT_DIR}/algorithms/symmCrypto/inc
)

add_executable(test_certX509 test_certX509.cpp)

target_link_libraries(test_certX509 ssl crypto algorithms library platform)