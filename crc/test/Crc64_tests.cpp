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
 * - one where the full sample array is provided to the Crc_CalculateCRC64 function
 * - one where the calculation is done with one call for each byte in the sample array
 */

TEST(Crc64Tests, CalculateCrc64_sample1) {
    uint64_t result = Crc_CalculateCRC64(sample1.data(), 4, 0xFFFFFFFFFFFFFFFF, true);
    EXPECT_EQ(result, sample1_crc64);
}

TEST(Crc64Tests, CalculateCrc64_sample1_incremental) {
    auto iter = sample1.begin();
    uint64_t result = Crc_CalculateCRC64(iter, 1, 0xFFFFFFFFFFFFFFFF, true);
    iter++;

    while (iter < sample1.end()) {
        result = Crc_CalculateCRC64(iter, 1, result, false);
        iter++;
    }

    EXPECT_EQ(result, sample1_crc64);
}

TEST(Crc64Tests, CalculateCrc64_sample2) {
    uint64_t result = Crc_CalculateCRC64(sample2.data(), 3, 0xFFFFFFFFFFFFFFFF, true);
    EXPECT_EQ(result, sample2_crc64);
}

TEST(Crc64Tests, CalculateCrc64_sample2_incremental) {
    auto iter = sample2.begin();
    uint64_t result = Crc_CalculateCRC64(iter, 1, 0xFFFFFFFFFFFFFFFF, true);
    iter++;

    while (iter < sample2.end()) {
        result = Crc_CalculateCRC64(iter, 1, result, false);
        iter++;
    }

    EXPECT_EQ(result, sample2_crc64);
}

TEST(Crc64Tests, CalculateCrc64_sample3) {
    uint64_t result = Crc_CalculateCRC64(sample3.data(), 4, 0xFFFFFFFFFFFFFFFF, true);
    EXPECT_EQ(result, sample3_crc64);
}

TEST(Crc64Tests, CalculateCrc64_sample3_incremental) {
    auto iter = sample3.begin();
    uint64_t result = Crc_CalculateCRC64(iter, 1, 0xFFFFFFFFFFFFFFFF, true);
    iter++;

    while (iter < sample3.end()) {
        result = Crc_CalculateCRC64(iter, 1, result, false);
        iter++;
    }

    EXPECT_EQ(result, sample3_crc64);
}

TEST(Crc64Tests, CalculateCrc64_sample4) {
    uint64_t result = Crc_CalculateCRC64(sample4.data(), 4, 0xFFFFFFFFFFFFFFFF, true);
    EXPECT_EQ(result, sample4_crc64);
}

TEST(Crc64Tests, CalculateCrc64_sample4_incremental) {
    auto iter = sample4.begin();
    uint64_t result = Crc_CalculateCRC64(iter, 1, 0xFFFFFFFFFFFFFFFF, true);
    iter++;

    while (iter < sample4.end()) {
        result = Crc_CalculateCRC64(iter, 1, result, false);
        iter++;
    }

    EXPECT_EQ(result, sample4_crc64);
}

TEST(Crc64Tests, CalculateCrc64_sample5) {
    uint64_t result = Crc_CalculateCRC64(sample5.data(), 9, 0xFFFFFFFFFFFFFFFF, true);
    EXPECT_EQ(result, sample5_crc64);
}

TEST(Crc64Tests, CalculateCrc64_sample5_incremental) {
    auto iter = sample5.begin();
    uint64_t result = Crc_CalculateCRC64(iter, 1, 0xFFFFFFFFFFFFFFFF, true);
    iter++;

    while (iter < sample5.end()) {
        result = Crc_CalculateCRC64(iter, 1, result, false);
        iter++;
    }

    EXPECT_EQ(result, sample5_crc64);
}

TEST(Crc64Tests, CalculateCrc64_sample6) {
    uint64_t result = Crc_CalculateCRC64(sample6.data(), 3, 0xFFFFFFFFFFFFFFFF, true);
    EXPECT_EQ(result, sample6_crc64);
}

TEST(Crc64Tests, CalculateCrc64_sample6_incremental) {
    auto iter = sample6.begin();
    uint64_t result = Crc_CalculateCRC64(iter, 1, 0xFFFFFFFFFFFFFFFF, true);
    iter++;

    while (iter < sample6.end()) {
        result = Crc_CalculateCRC64(iter, 1, result, false);
        iter++;
    }

    EXPECT_EQ(result, sample6_crc64);
}

TEST(Crc64Tests, CalculateCrc64_sample7) {
    uint64_t result = Crc_CalculateCRC64(sample7.data(), 4, 0xFFFFFFFFFFFFFFFF, true);
    EXPECT_EQ(result, sample7_crc64);
}

TEST(Crc64Tests, CalculateCrc64_sample7_incremental) {
    auto iter = sample7.begin();
    uint64_t result = Crc_CalculateCRC64(iter, 1, 0xFFFFFFFFFFFFFFFF, true);
    iter++;

    while (iter < sample7.end()) {
        result = Crc_CalculateCRC64(iter, 1, result, false);
        iter++;
    }

    EXPECT_EQ(result, sample7_crc64);
}
