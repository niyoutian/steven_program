project(test_cjson)

include_directories(
)

file(GLOB SRC_FILE
    "*.c"
)


add_executable(test_cjson ${SRC_FILE})
target_link_libraries(test_cjson m)