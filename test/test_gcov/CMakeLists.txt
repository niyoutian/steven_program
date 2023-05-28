project(test_gcov)

include_directories(
)

file(GLOB SRC_FILE
    "*.c"
)


add_executable(test_gcov ${SRC_FILE})