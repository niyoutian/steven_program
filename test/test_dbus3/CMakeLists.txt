project(test_dbus3)

include_directories(
    /home/steven/study/dbus_pc/install/include/dbus-1.0
    /home/steven/study/dbus_pc/install/lib/dbus-1.0/include
)

add_executable(dbus3_client dbus_client.c dbus_func.c)
add_executable(dbus3_server dbus_server.c dbus_func.c)

target_link_libraries(dbus3_client dbus-1)
target_link_libraries(dbus3_server dbus-1)
