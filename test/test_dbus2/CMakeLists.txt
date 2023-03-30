cmake_minimum_required (VERSION 2.8)

project(test_dbus2)

include_directories(
    /home/steven/study/dbus_pc/install/include/dbus-1.0
    /home/steven/study/dbus_pc/install/lib/dbus-1.0/include
)

add_executable(dbus2_client dbus_client.c dbus_func.c)
add_executable(dbus2_server dbus_server.c dbus_func.c)

target_link_libraries(dbus2_client dbus-1)
target_link_libraries(dbus2_server dbus-1)
