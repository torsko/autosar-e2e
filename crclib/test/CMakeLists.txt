
add_executable(Crc8_test Crc8_test.cpp)
target_link_libraries(Crc8_test PUBLIC crclib gtest_main)
gtest_discover_tests(Crc8_test)
