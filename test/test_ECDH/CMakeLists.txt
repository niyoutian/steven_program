project(test_ECDH)

SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")

include_directories(
)

add_executable(test_ECDH test_ECDH.cpp)

target_link_libraries(test_ECDH ssl crypto)
