project(test_dbus3)

include_directories(
    /usr/include/glib-2.0/glib
    /usr/include/glib-2.0
    /usr/lib/x86_64-linux-gnu/glib-2.0/include
    /home/steven/study/dbus_pc/install/include/dbus-1.0
    /home/steven/study/dbus_pc/install/lib/dbus-1.0/include
)

add_executable(dbus_ping_send dbus_ping_send.c)
add_executable(dbus_ping_listen dbus_ping_listen.c)

target_link_libraries(dbus_ping_send dbus-1 dbus-glib-1 glib-2.0)
target_link_libraries(dbus_ping_listen dbus-1 dbus-glib-1 glib-2.0)
