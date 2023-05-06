project(test_dbus)

include_directories(
    /home/steven/study/dbus_pc/install/include/dbus-1.0
    /home/steven/study/dbus_pc/install/lib/dbus-1.0/include
)

add_executable(dbus_client dbus_client.c dbus_func.c)
add_executable(dbus_server dbus_server.c dbus_func.c)

target_link_libraries(dbus_client dbus-1)
target_link_libraries(dbus_server dbus-1)
