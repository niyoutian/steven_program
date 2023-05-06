project(someip)

SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")

FILE(GLOB SOMEIP_SD_SOURCE "sd/*.cpp")

FILE(GLOB SOMEIP_SOURCE "someip/*.cpp")

include_directories(
    ${PROJECT_ROOT_DIR}/platform/common/inc
    ${PROJECT_ROOT_DIR}/library/credentials/inc
	${PROJECT_ROOT_DIR}/algorithms/symmCrypto/inc
    ${PROJECT_ROOT_DIR}/library/xfrm/inc
    ${PROJECT_ROOT_DIR}/app/someip/common
)

# add_library(someip_sd SHARED ${SOMEIP_SD_SOURCE})
add_library(someip SHARED ${SOMEIP_SOURCE})
