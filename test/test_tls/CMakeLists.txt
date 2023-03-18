cmake_minimum_required (VERSION 2.8)

project(test_tls)

include_directories(
)

SET(CMAKE_C_FLAGS_DEBUG "$ENV{CFLAGS} -O0 -Wall -g2 -ggdb")

add_executable(tls_client tls_client.c tls_func.c)
add_executable(tls_server tls_server.c tls_func.c)

target_link_libraries(tls_client ssl crypto)
target_link_libraries(tls_server ssl crypto)