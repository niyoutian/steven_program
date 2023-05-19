project(test_getopt)

include_directories(
)

file(GLOB SRC_FILE
    "*.c"
)


add_executable(test_getopt ${SRC_FILE})