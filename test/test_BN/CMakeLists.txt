project(test_BN)

SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")

include_directories(
)

add_executable(test_BN test_BN.cpp)
add_executable(test_BN2 test_BN2.cpp)

target_link_libraries(test_BN ssl crypto)
target_link_libraries(test_BN2 ssl crypto)
