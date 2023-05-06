project(test_mydbus)

include_directories(
    ${PROJECT_ROOT_DIR}/platform/common/inc
    ${PROJECT_ROOT_DIR}/platform/process/inc
    /home/steven/study/dbus_pc/install/include/dbus-1.0
    /home/steven/study/dbus_pc/install/lib/dbus-1.0/include
)

SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")

add_executable(test_mydbus test_mydbus.cpp)
target_link_libraries(test_mydbus algorithms library platform dbus-1) 
