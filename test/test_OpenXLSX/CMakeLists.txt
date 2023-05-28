project(test_xlsx)

# include_directories(
#     /home/steven/study/excel/OpenXLSX/OpenXLSX
# )

file(GLOB SRC_FILE
    "*.cpp"
)

add_executable(test_xlsx ${SRC_FILE})
target_link_directories(test_xlsx PRIVATE /home/steven/study/excel/OpenXLSX/build_dir/output)
target_include_directories(test_xlsx PRIVATE /home/steven/study/excel/OpenXLSX/OpenXLSX
                                            /home/steven/study/excel/OpenXLSX/build_dir/OpenXLSX)
target_link_libraries(test_xlsx OpenXLSX)