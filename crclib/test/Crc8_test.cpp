extern "C" {
    #include "Crc.h"
}

#include <gtest/gtest.h>

/**
 * These tests will make use of the sample results provided for each algorithm in
 * https://www.autosar.org/fileadmin/user_upload/standards/classic/20-11/AUTOSAR_SWS_CRCLibrary.pdf
 *
 * Each sample will have two tests:
 * - one where the full sample array is provided to the Crc_CalculateCRC8 function
 * - one where the calculation is done with one call for each byte in the sample array
 */

namespace {
    const std::array<uint8_t, 4> crc8_sample1{0, 0, 0, 0};
    const uint8_t crc8_sample1_expected_crc{0x59};

    const std::array<uint8_t, 3> crc8_sample2{0xF2, 0x01, 0x83};
    const uint8_t crc8_sample2_expected_crc = 0x37;

    const std::array<uint8_t, 4> crc8_sample3{0x0F, 0xAA, 0x00, 0x55};
    const uint8_t crc8_sample3_expected_crc = 0x79;

    const std::array<uint8_t, 4> crc8_sample4{0x00, 0xFF, 0x55, 0x11};
    const uint8_t crc8_sample4_expected_crc = 0xB8;

    const std::array<uint8_t, 9> crc8_sample5{0x33, 0x22, 0x55, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
    const uint8_t crc8_sample5_expected_crc = 0xCB;

    const std::array<uint8_t, 3> crc8_sample6{0x92, 0x6B, 0x55};
    const uint8_t crc8_sample6_expected_crc = 0x8C;

    const std::array<uint8_t, 4> crc8_sample7{0xFF, 0xFF, 0xFF, 0xFF};
    const uint8_t crc8_sample7_expected_crc = 0x74;
}

TEST(CrcTests, CalculateCRC8_sample1) {
    uint8_t result = Crc_CalculateCRC8(crc8_sample1.data(), 4, 0xFF, true);
    EXPECT_EQ(result, crc8_sample1_expected_crc);
}

TEST(CrcTests, CalculateCRC8_sample1_incremental) {
    auto iter = crc8_sample1.begin();
    uint8_t result = Crc_CalculateCRC8(iter, 1, 0xFF, true);
    iter++;

    while (iter < crc8_sample1.end()) {
        result = Crc_CalculateCRC8(iter, 1, result, false);
        iter++;
    }

    EXPECT_EQ(result, crc8_sample1_expected_crc);
}

TEST(CrcTests, CalculateCRC8_sample2) {
    uint8_t result = Crc_CalculateCRC8(crc8_sample2.data(), 3, 0xFF, true);
    EXPECT_EQ(result, crc8_sample2_expected_crc);
}

TEST(CrcTests, CalculateCRC8_sample2_incremental) {
    auto iter = crc8_sample2.begin();
    uint8_t result = Crc_CalculateCRC8(iter, 1, 0xFF, true);
    iter++;

    while (iter < crc8_sample2.end()) {
        result = Crc_CalculateCRC8(iter, 1, result, false);
        iter++;
    }

    EXPECT_EQ(result, crc8_sample2_expected_crc);
}

TEST(CrcTests, CalculateCRC8_sample3) {
    uint8_t result = Crc_CalculateCRC8(crc8_sample3.data(), 4, 0xFF, true);
    EXPECT_EQ(result, crc8_sample3_expected_crc);
}

TEST(CrcTests, CalculateCRC8_sample3_incremental) {
    auto iter = crc8_sample3.begin();
    uint8_t result = Crc_CalculateCRC8(iter, 1, 0xFF, true);
    iter++;

    while (iter < crc8_sample3.end()) {
        result = Crc_CalculateCRC8(iter, 1, result, false);
        iter++;
    }

    EXPECT_EQ(result, crc8_sample3_expected_crc);
}

TEST(CrcTests, CalculateCRC8_sample4) {
    uint8_t result = Crc_CalculateCRC8(crc8_sample4.data(), 4, 0xFF, true);
    EXPECT_EQ(result, crc8_sample4_expected_crc);
}

TEST(CrcTests, CalculateCRC8_sample4_incremental) {
    auto iter = crc8_sample4.begin();
    uint8_t result = Crc_CalculateCRC8(iter, 1, 0xFF, true);
    iter++;

    while (iter < crc8_sample4.end()) {
        result = Crc_CalculateCRC8(iter, 1, result, false);
        iter++;
    }

    EXPECT_EQ(result, crc8_sample4_expected_crc);
}

TEST(CrcTests, CalculateCRC8_sample5) {
    uint8_t result = Crc_CalculateCRC8(crc8_sample5.data(), 9, 0xFF, true);
    EXPECT_EQ(result, crc8_sample5_expected_crc);
}

TEST(CrcTests, CalculateCRC8_sample5_incremental) {
    auto iter = crc8_sample5.begin();
    uint8_t result = Crc_CalculateCRC8(iter, 1, 0xFF, true);
    iter++;

    while (iter < crc8_sample5.end()) {
        result = Crc_CalculateCRC8(iter, 1, result, false);
        iter++;
    }

    EXPECT_EQ(result, crc8_sample5_expected_crc);
}

TEST(CrcTests, CalculateCRC8_sample6) {
    uint8_t result = Crc_CalculateCRC8(crc8_sample6.data(), 3, 0xFF, true);
    EXPECT_EQ(result, crc8_sample6_expected_crc);
}

TEST(CrcTests, CalculateCRC8_sample6_incremental) {
    auto iter = crc8_sample6.begin();
    uint8_t result = Crc_CalculateCRC8(iter, 1, 0xFF, true);
    iter++;

    while (iter < crc8_sample6.end()) {
        result = Crc_CalculateCRC8(iter, 1, result, false);
        iter++;
    }

    EXPECT_EQ(result, crc8_sample6_expected_crc);
}

TEST(CrcTests, CalculateCRC8_sample7) {
    uint8_t result = Crc_CalculateCRC8(crc8_sample7.data(), 4, 0xFF, true);
    EXPECT_EQ(result, crc8_sample7_expected_crc);
}

TEST(CrcTests, CalculateCRC8_sample7_incremental) {
    auto iter = crc8_sample7.begin();
    uint8_t result = Crc_CalculateCRC8(iter, 1, 0xFF, true);
    iter++;

    while (iter < crc8_sample7.end()) {
        result = Crc_CalculateCRC8(iter, 1, result, false);
        iter++;
    }

    EXPECT_EQ(result, crc8_sample7_expected_crc);
}
