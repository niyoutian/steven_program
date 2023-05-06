project(test_tcp_nonblock)

include_directories(
)

add_executable(tcp_client tcp_client.c tcp_func.c)
add_executable(tcp_server tcp_server.c tcp_func.c)

#target_link_libraries(tcp_client ssl crypto)
#target_link_libraries(tcp_server ssl crypto)
