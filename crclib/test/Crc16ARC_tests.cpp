extern "C" {
    #include "Crc.h"
}

#include "test_data.h"

#include <gtest/gtest.h>

/**
 * These tests will make use of the sample results provided for each algorithm in
 * https://www.autosar.org/fileadmin/user_upload/standards/classic/20-11/AUTOSAR_SWS_CRCLibrary.pdf
 *
 * The sample data is specified in test_data.h
 * There are seven std::array<uint8_t, N> named sample1 through sample7 containing the samples
 * Each sample has an expected CRC for each CRC-calculation method called sampleN_crc8, sampleN_crc16 et.c
 *
 * Each sample will have two tests:
 * - one where the full sample array is provided to the Crc_CalculateCRC16ARC function
 * - one where the calculation is done with one call for each byte in the sample array
 */

TEST(Crc16ARCTests, CalculateCRC16ARC_sample1) {
    uint16_t result = Crc_CalculateCRC16ARC(sample1.data(), 4, 0xFFFF, true);
    EXPECT_EQ(result, sample1_crc16ARC);
}

TEST(Crc16ARCTests, CalculateCRC16ARC_sample1_incremental) {
    auto iter = sample1.begin();
    uint16_t result = Crc_CalculateCRC16ARC(iter, 1, 0xFFFF, true);
    iter++;

    while (iter < sample1.end()) {
        result = Crc_CalculateCRC16ARC(iter, 1, result, false);
        iter++;
    }

    EXPECT_EQ(result, sample1_crc16ARC);
}

TEST(Crc16ARCTests, CalculateCRC16ARC_sample2) {
    uint16_t result = Crc_CalculateCRC16ARC(sample2.data(), 3, 0xFFFF, true);
    EXPECT_EQ(result, sample2_crc16ARC);
}

TEST(Crc16ARCTests, CalculateCRC16ARC_sample2_incremental) {
    auto iter = sample2.begin();
    uint16_t result = Crc_CalculateCRC16ARC(iter, 1, 0xFFFF, true);
    iter++;

    while (iter < sample2.end()) {
        result = Crc_CalculateCRC16ARC(iter, 1, result, false);
        iter++;
    }

    EXPECT_EQ(result, sample2_crc16ARC);
}

TEST(Crc16ARCTests, CalculateCRC16ARC_sample3) {
    uint16_t result = Crc_CalculateCRC16ARC(sample3.data(), 4, 0xFFFF, true);
    EXPECT_EQ(result, sample3_crc16ARC);
}

TEST(Crc16ARCTests, CalculateCRC16ARC_sample3_incremental) {
    auto iter = sample3.begin();
    uint16_t result = Crc_CalculateCRC16ARC(iter, 1, 0xFFFF, true);
    iter++;

    while (iter < sample3.end()) {
        result = Crc_CalculateCRC16ARC(iter, 1, result, false);
        iter++;
    }

    EXPECT_EQ(result, sample3_crc16ARC);
}

TEST(Crc16ARCTests, CalculateCRC16ARC_sample4) {
    uint16_t result = Crc_CalculateCRC16ARC(sample4.data(), 4, 0xFFFF, true);
    EXPECT_EQ(result, sample4_crc16ARC);
}

TEST(Crc16ARCTests, CalculateCRC16ARC_sample4_incremental) {
    auto iter = sample4.begin();
    uint16_t result = Crc_CalculateCRC16ARC(iter, 1, 0xFFFF, true);
    iter++;

    while (iter < sample4.end()) {
        result = Crc_CalculateCRC16ARC(iter, 1, result, false);
        iter++;
    }

    EXPECT_EQ(result, sample4_crc16ARC);
}

TEST(Crc16ARCTests, CalculateCRC16ARC_sample5) {
    uint16_t result = Crc_CalculateCRC16ARC(sample5.data(), 9, 0xFFFF, true);
    EXPECT_EQ(result, sample5_crc16ARC);
}

TEST(Crc16ARCTests, CalculateCRC16ARC_sample5_incremental) {
    auto iter = sample5.begin();
    uint16_t result = Crc_CalculateCRC16ARC(iter, 1, 0xFFFF, true);
    iter++;

    while (iter < sample5.end()) {
        result = Crc_CalculateCRC16ARC(iter, 1, result, false);
        iter++;
    }

    EXPECT_EQ(result, sample5_crc16ARC);
}

TEST(Crc16ARCTests, CalculateCRC16ARC_sample6) {
    uint16_t result = Crc_CalculateCRC16ARC(sample6.data(), 3, 0xFFFF, true);
    EXPECT_EQ(result, sample6_crc16ARC);
}

TEST(Crc16ARCTests, CalculateCRC16ARC_sample6_incremental) {
    auto iter = sample6.begin();
    uint16_t result = Crc_CalculateCRC16ARC(iter, 1, 0xFFFF, true);
    iter++;

    while (iter < sample6.end()) {
        result = Crc_CalculateCRC16ARC(iter, 1, result, false);
        iter++;
    }

    EXPECT_EQ(result, sample6_crc16ARC);
}

TEST(Crc16ARCTests, CalculateCRC16ARC_sample7) {
    uint16_t result = Crc_CalculateCRC16ARC(sample7.data(), 4, 0xFFFF, true);
    EXPECT_EQ(result, sample7_crc16ARC);
}

TEST(Crc16ARCTests, CalculateCRC16ARC_sample7_incremental) {
    auto iter = sample7.begin();
    uint16_t result = Crc_CalculateCRC16ARC(iter, 1, 0xFFFF, true);
    iter++;

    while (iter < sample7.end()) {
        result = Crc_CalculateCRC16ARC(iter, 1, result, false);
        iter++;
    }

    EXPECT_EQ(result, sample7_crc16ARC);
}
