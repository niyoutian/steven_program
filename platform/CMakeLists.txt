project(platform)

FILE(GLOB src_files
    "common/src/*.cpp"
    "net/src/*.cpp"
    "process/src/*.cpp"
)

# list(REMOVE_ITEM src_files ${PROJECT_SOURCE_DIR}/net/src/mxEngineCurl.cpp ${PROJECT_SOURCE_DIR}/common/src/mxTimer.cpp)
list(REMOVE_ITEM src_files ${PROJECT_SOURCE_DIR}/net/src/mxEngineCurl.cpp)

SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")




include_directories(
    ${PROJECT_SOURCE_DIR}/common/inc
    ${PROJECT_SOURCE_DIR}/net/inc
    ${PROJECT_SOURCE_DIR}/process/inc
    /home/steven/study/dbus_pc/install/include/dbus-1.0
    /home/steven/study/dbus_pc/install/lib/dbus-1.0/include
)

#target_link_libraries(${PROJECT_NAME} pthread)
add_library(${PROJECT_NAME} SHARED ${src_files})
target_link_libraries(${PROJECT_NAME} dbus-1)

install(FILES common/inc/mxStatus.h DESTINATION ${PROJECT_BINARY_DIR}/../include)
