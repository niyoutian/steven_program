project(test_multicast)

include_directories(
)

add_executable(multi_server test_multi_server.c test_multi_func.c)
add_executable(multi_client test_multi_client.c test_multi_func.c)
