project(test_ikev2)

SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")

include_directories(
    ${PROJECT_ROOT_DIR}/platform/common/inc
	${PROJECT_ROOT_DIR}/algorithms/symmCrypto/inc
    ${PROJECT_ROOT_DIR}/library/xfrm/inc
    ${PROJECT_ROOT_DIR}/app/test_net
    ${PROJECT_ROOT_DIR}/platform/net/inc
)

add_executable(test_ikeMgr ikeMgr.cpp)

target_link_libraries(test_ikeMgr algorithms library platform)
