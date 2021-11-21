
add_executable(Crc8_tests Crc8_tests.cpp)
target_link_libraries(Crc8_tests PUBLIC crc gtest_main)
gtest_discover_tests(Crc8_tests)

add_executable(Crc8H2F_tests Crc8H2F_tests.cpp)
target_link_libraries(Crc8H2F_tests PUBLIC crc gtest_main)
gtest_discover_tests(Crc8H2F_tests)

add_executable(Crc16_tests Crc16_tests.cpp)
target_link_libraries(Crc16_tests PUBLIC crc gtest_main)
gtest_discover_tests(Crc16_tests)

add_executable(Crc16ARC_tests Crc16ARC_tests.cpp)
target_link_libraries(Crc16ARC_tests PUBLIC crc gtest_main)
gtest_discover_tests(Crc16ARC_tests)

add_executable(Crc32_tests Crc32_tests.cpp)
target_link_libraries(Crc32_tests PUBLIC crc gtest_main)
gtest_discover_tests(Crc32_tests)

add_executable(Crc32P4_tests Crc32P4_tests.cpp)
target_link_libraries(Crc32P4_tests PUBLIC crc gtest_main)
gtest_discover_tests(Crc32P4_tests)

add_executable(Crc64_tests Crc64_tests.cpp)
target_link_libraries(Crc64_tests PUBLIC crc gtest_main)
gtest_discover_tests(Crc64_tests)
