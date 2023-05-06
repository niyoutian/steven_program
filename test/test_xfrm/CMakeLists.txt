project(test_xfrm)

SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")

include_directories(
    ${PROJECT_ROOT_DIR}/platform/common/inc
    ${PROJECT_ROOT_DIR}/library/credentials/inc
	${PROJECT_ROOT_DIR}/algorithms/symmCrypto/inc
    ${PROJECT_ROOT_DIR}/library/xfrm/inc
)

add_executable(test_netlink test_netlink.cpp)

target_link_libraries(test_netlink algorithms library platform)
