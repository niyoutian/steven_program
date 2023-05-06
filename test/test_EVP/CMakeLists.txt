project(test_EVP)

SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")

include_directories(
    ${PROJECT_ROOT_DIR}/platform/common/inc
    ${PROJECT_ROOT_DIR}/library/credentials/inc
	${PROJECT_ROOT_DIR}/algorithms/symmCrypto/inc
	${PROJECT_ROOT_DIR}/algorithms/hash/inc
)

add_executable(test_3des test_3des.cpp)
add_executable(test_hash test_hash.cpp)
add_executable(test_hmac test_hmac.cpp)

target_link_libraries(test_3des algorithms library platform)
target_link_libraries(test_hash algorithms library platform)
target_link_libraries(test_hmac algorithms library platform)
