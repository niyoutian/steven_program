project(test_timer)

include_directories(
    ${PROJECT_ROOT_DIR}/platform/common/inc
)

SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")

add_executable(test_miniTimer test_miniTimer.cpp)
target_link_libraries(test_miniTimer algorithms library platform)
