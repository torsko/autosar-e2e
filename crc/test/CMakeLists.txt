# Since the API is identical and only the implementation differs
# the same test can be applied for both the small and the fast
# CRC library implementation.


##### Tests for the crc_small library #####
set(CRC_LIB crc_small)

# Small CRC8 tests
add_executable(Crc8_small_tests Crc8_tests.cpp)
target_link_libraries(Crc8_small_tests PUBLIC ${CRC_LIB} gtest_main)
gtest_discover_tests(Crc8_small_tests)

# Small CRC8H2F tests
add_executable(Crc8H2F_small_tests Crc8H2F_tests.cpp)
target_link_libraries(Crc8H2F_small_tests PUBLIC ${CRC_LIB} gtest_main)
gtest_discover_tests(Crc8H2F_small_tests)

# Small CRC16 tests
add_executable(Crc16_small_tests Crc16_tests.cpp)
target_link_libraries(Crc16_small_tests PUBLIC ${CRC_LIB} gtest_main)
gtest_discover_tests(Crc16_small_tests)

# Small CRC16ARC tests
add_executable(Crc16ARC_small_tests Crc16ARC_tests.cpp)
target_link_libraries(Crc16ARC_small_tests PUBLIC ${CRC_LIB} gtest_main)
gtest_discover_tests(Crc16ARC_small_tests)

# Small CRC32 tests
add_executable(Crc32_small_tests Crc32_tests.cpp)
target_link_libraries(Crc32_small_tests PUBLIC ${CRC_LIB} gtest_main)
gtest_discover_tests(Crc32_small_tests)

# Small CRC32P4 tests
add_executable(Crc32P4_small_tests Crc32P4_tests.cpp)
target_link_libraries(Crc32P4_small_tests PUBLIC ${CRC_LIB} gtest_main)
gtest_discover_tests(Crc32P4_small_tests)

# Small CRC64 tests
add_executable(Crc64_small_tests Crc64_tests.cpp)
target_link_libraries(Crc64_small_tests PUBLIC ${CRC_LIB} gtest_main)
gtest_discover_tests(Crc64_small_tests)


##### Tests for the crc_fast library #####
set(CRC_LIB crc_fast)

# Fast CRC8 tests
add_executable(Crc8_fast_tests Crc8_tests.cpp)
target_link_libraries(Crc8_fast_tests PUBLIC ${CRC_LIB} gtest_main)
gtest_discover_tests(Crc8_fast_tests)

# Fast CRC8H2F tests
add_executable(Crc8H2F_fast_tests Crc8H2F_tests.cpp)
target_link_libraries(Crc8H2F_fast_tests PUBLIC ${CRC_LIB} gtest_main)
gtest_discover_tests(Crc8H2F_fast_tests)

# Fast CRC16 tests
add_executable(Crc16_fast_tests Crc16_tests.cpp)
target_link_libraries(Crc16_fast_tests PUBLIC ${CRC_LIB} gtest_main)
gtest_discover_tests(Crc16_fast_tests)

# Fast CRC16ARC tests
add_executable(Crc16ARC_fast_tests Crc16ARC_tests.cpp)
target_link_libraries(Crc16ARC_fast_tests PUBLIC ${CRC_LIB} gtest_main)
gtest_discover_tests(Crc16ARC_fast_tests)

# Fast CRC32 tests
add_executable(Crc32_fast_tests Crc32_tests.cpp)
target_link_libraries(Crc32_fast_tests PUBLIC ${CRC_LIB} gtest_main)
gtest_discover_tests(Crc32_fast_tests)

# Fast CRC32P4 tests
add_executable(Crc32P4_fast_tests Crc32P4_tests.cpp)
target_link_libraries(Crc32P4_fast_tests PUBLIC ${CRC_LIB} gtest_main)
gtest_discover_tests(Crc32P4_fast_tests)

# Fast CRC64 tests
add_executable(Crc64_fast_tests Crc64_tests.cpp)
target_link_libraries(Crc64_fast_tests PUBLIC ${CRC_LIB} gtest_main)
gtest_discover_tests(Crc64_fast_tests)
