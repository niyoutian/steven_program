project(test_net)

SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")

include_directories(
    ${PROJECT_ROOT_DIR}/platform/common/inc
    ${PROJECT_ROOT_DIR}/library/credentials/inc
	${PROJECT_ROOT_DIR}/algorithms/symmCrypto/inc
    ${PROJECT_ROOT_DIR}/library/xfrm/inc
    ${PROJECT_ROOT_DIR}/platform/net/inc
)

add_executable(test_net test_net.cpp)

target_link_libraries(test_net algorithms library platform)
