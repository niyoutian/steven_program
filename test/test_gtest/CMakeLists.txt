project(test_gtest)

include_directories(
    /home/steven/study/googletest/googletest/include
)

add_executable(test_gtest test_gtest.cpp)
# target_link_directories(test_gtest PRIVATE /home/steven/study/googletest/build_dir/lib)
target_link_directories(test_gtest PRIVATE /home/steven/study/googletest/build_dir/lib)
target_link_libraries(test_gtest gmock  gmock_main  gtest  gtest_main)