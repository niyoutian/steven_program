project(test_mysql)

SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")

include_directories(
    ${PROJECT_ROOT_DIR}/platform/common/inc
    ${PROJECT_ROOT_DIR}/library/credentials/inc
	${PROJECT_ROOT_DIR}/algorithms/symmCrypto/inc
    ${PROJECT_ROOT_DIR}/library/xfrm/inc
    ${PROJECT_ROOT_DIR}/platform/mysql/inc
)

add_executable(test_mysql test_mysql.cpp)

target_link_libraries(test_mysql algorithms library platform)
