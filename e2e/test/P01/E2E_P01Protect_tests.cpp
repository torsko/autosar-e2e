extern "C" {
    #include "E2E_P01.h"
}

#include <gtest/gtest.h>

class P01Protect : public ::testing::Test {
protected:
    void SetUp() override {
        SetExampleConfig();
        SetExampleState();
    }

    void SetExampleConfig() {
        config_.CounterOffset = 8;
        config_.CRCOffset = 0;
        config_.DataID = 0x123;
        config_.DataIDMode = E2E_P01_DATAID_BOTH;
        // Actual DataIDNibbleOffset in the example is 12 but
        // it is only used there once so setting it manually for that test instead
        config_.DataIDNibbleOffset = 0;
        config_.DataLength = 64;
        config_.MaxDeltaCounterInit = 1;
        config_.MaxNoNewOrRepeatedData = 15;
        config_.SyncCounterInit = 0;
    }

    void SetExampleState() {
        state_.Counter = 0;
    }

    uint8_t buffer_[8];
    E2E_P01ConfigType config_;
    E2E_P01ProtectStateType state_;

};

TEST_F(P01Protect, ByteAlignedCounterWriteKeepsHighNibble) {
    // Add old data in the target byte so we can verify
    // that it isn't overwritten.
    buffer_[1] = 0xD0;

    config_.CounterOffset = 8;
    state_.Counter = 0x0C;

    const uint8_t expectedCounterByteContents = 0xDC;

    Std_ReturnType result = E2E_P01Protect(&config_, &state_, buffer_);
    EXPECT_EQ(result, E2E_E_OK);
    EXPECT_EQ(buffer_[1], expectedCounterByteContents);
}

TEST_F(P01Protect, MidByteAlignedCounterWriteKeepsLowNibble) {
    // Add old data in the target byte so we can verify
    // that it isn't overwritten.
    buffer_[1] = 0x0C;
    config_.CounterOffset = 12; // High nibble of second byte
    state_.Counter = 0x0A;

    const uint8_t expectedCounterByteContents = 0xAC;

    Std_ReturnType result = E2E_P01Protect(&config_, &state_, buffer_);
    EXPECT_EQ(result, E2E_E_OK);
    EXPECT_EQ(buffer_[1], expectedCounterByteContents);
}

TEST_F(P01Protect, CounterIsIncrementedAfterEachProtect) {
    config_.CounterOffset = 0;
    state_.Counter = 0x0;

    Std_ReturnType result = E2E_P01Protect(&config_, &state_, buffer_);
    EXPECT_EQ(result, E2E_E_OK);
    EXPECT_EQ(state_.Counter, 0x1);

    result = E2E_P01Protect(&config_, &state_, buffer_);
    EXPECT_EQ(result, E2E_E_OK);
    EXPECT_EQ(state_.Counter, 0x2);

    result = E2E_P01Protect(&config_, &state_, buffer_);
    EXPECT_EQ(result, E2E_E_OK);
    EXPECT_EQ(state_.Counter, 0x3);
}

TEST_F(P01Protect, CounterRestartsAtZeroAfter14) {
    config_.CounterOffset = 8;
    state_.Counter = 0xE;

    Std_ReturnType result = E2E_P01Protect(&config_, &state_, buffer_);
    EXPECT_EQ(result, E2E_E_OK);
    EXPECT_EQ(state_.Counter, 0x0);
}

TEST_F(P01Protect, ByteAlignedDataIDNibbleKeepsOldData) {
    config_.DataIDMode = E2E_P01_DATAID_NIBBLE;
    config_.DataIDNibbleOffset = 8;
    config_.DataID = 0x0ABC;
    buffer_[1] = 0x20;

    Std_ReturnType result = E2E_P01Protect(&config_, &state_, buffer_);
    EXPECT_EQ(result, E2E_E_OK);
    EXPECT_EQ(buffer_[1], 0x2A);
}

TEST_F(P01Protect, MidByteAlignedDataIDNibbleKeepsOldData) {
    // Using third byte for this as to not interfere with counter config
    config_.DataIDMode = E2E_P01_DATAID_NIBBLE;
    config_.DataIDNibbleOffset = 20;
    config_.DataID = 0x0ABC;
    buffer_[2] = 0x02;

    Std_ReturnType result = E2E_P01Protect(&config_, &state_, buffer_);
    EXPECT_EQ(result, E2E_E_OK);
    EXPECT_EQ(buffer_[2], 0xA2);
}

// Using the examples in AUTOSAR_PRS_E2EProtocol chapter 6.3.5

TEST_F(P01Protect, ProtocolExample_DATAID_BOTH) {
    const uint8_t expected_buffer1[8]{0xCC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t buffer1[8]{0, 0, 0, 0, 0, 0, 0, 0};
    const uint8_t expected_buffer2[8]{0x91, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t buffer2[8]{0, 0, 0, 0, 0, 0, 0, 0};

    Std_ReturnType result = E2E_P01Protect(&config_, &state_, buffer1);
    EXPECT_EQ(result, E2E_E_OK);
    for (size_t i = 0; i<8; ++i) {
        EXPECT_EQ(buffer1[i], expected_buffer1[i]);
    }

    ASSERT_EQ(state_.Counter, 1);

    result = E2E_P01Protect(&config_, &state_, buffer2);
    EXPECT_EQ(result, E2E_E_OK);
    for (size_t i = 0; i<8; ++i) {
        EXPECT_EQ(buffer2[i], expected_buffer2[i]);
    }
}

TEST_F(P01Protect, ProtocolExample_DATAID_ALT) {
    const uint8_t expected_buffer1[8]{0xCE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t buffer1[8]{0, 0, 0, 0, 0, 0, 0, 0};
    const uint8_t expected_buffer2[8]{0x02, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t buffer2[8]{0, 0, 0, 0, 0, 0, 0, 0};

    config_.DataIDMode = E2E_P01_DATAID_ALT;

    Std_ReturnType result = E2E_P01Protect(&config_, &state_, buffer1);
    EXPECT_EQ(result, E2E_E_OK);
    for (size_t i = 0; i<8; ++i) {
        EXPECT_EQ(buffer1[i], expected_buffer1[i]);
    }

    ASSERT_EQ(state_.Counter, 1);

    result = E2E_P01Protect(&config_, &state_, buffer2);
    EXPECT_EQ(result, E2E_E_OK);
    for (size_t i = 0; i<8; ++i) {
        EXPECT_EQ(buffer2[i], expected_buffer2[i]);
    }
}

TEST_F(P01Protect, ProtocolExample_DATAID_LOW) {
    const uint8_t expected_buffer1[8]{0xCE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t buffer1[8]{0, 0, 0, 0, 0, 0, 0, 0};
    const uint8_t expected_buffer2[8]{0x93, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t buffer2[8]{0, 0, 0, 0, 0, 0, 0, 0};

    config_.DataIDMode = E2E_P01_DATAID_LOW;

    Std_ReturnType result = E2E_P01Protect(&config_, &state_, buffer1);
    EXPECT_EQ(result, E2E_E_OK);
    for (size_t i = 0; i<8; ++i) {
        EXPECT_EQ(buffer1[i], expected_buffer1[i]);
    }

    ASSERT_EQ(state_.Counter, 1);

    result = E2E_P01Protect(&config_, &state_, buffer2);
    EXPECT_EQ(result, E2E_E_OK);
    for (size_t i = 0; i<8; ++i) {
        EXPECT_EQ(buffer2[i], expected_buffer2[i]);
    }
}

TEST_F(P01Protect, ProtocolExample_DATAID_NIBBLE) {
    const uint8_t expected_buffer1[8]{0x2A, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t buffer1[8]{0, 0, 0, 0, 0, 0, 0, 0};
    const uint8_t expected_buffer2[8]{0x77, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    uint8_t buffer2[8]{0, 0, 0, 0, 0, 0, 0, 0};

    config_.DataIDMode = E2E_P01_DATAID_NIBBLE;
    config_.DataIDNibbleOffset = 12;

    Std_ReturnType result = E2E_P01Protect(&config_, &state_, buffer1);
    EXPECT_EQ(result, E2E_E_OK);
    for (size_t i = 0; i<8; ++i) {
        EXPECT_EQ(buffer1[i], expected_buffer1[i]);
    }

    ASSERT_EQ(state_.Counter, 1);

    result = E2E_P01Protect(&config_, &state_, buffer2);
    EXPECT_EQ(result, E2E_E_OK);
    for (size_t i = 0; i<8; ++i) {
        EXPECT_EQ(buffer2[i], expected_buffer2[i]);
    }
}

// Test having the CRC in the last byte of the payload
TEST_F(P01Protect, ProtocolExample_DATAID_NIBBLE_crcInLastByte) {
    const uint8_t expected_buffer1[8]{0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2A};
    uint8_t buffer1[8]{0, 0, 0, 0, 0, 0, 0, 0};
    const uint8_t expected_buffer2[8]{0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x77};
    uint8_t buffer2[8]{0, 0, 0, 0, 0, 0, 0, 0};

    config_.DataIDMode = E2E_P01_DATAID_NIBBLE;
    config_.DataIDNibbleOffset = 4;
    config_.CounterOffset = 0;
    config_.CRCOffset = 56;

    Std_ReturnType result = E2E_P01Protect(&config_, &state_, buffer1);
    EXPECT_EQ(result, E2E_E_OK);
    for (size_t i = 0; i<8; ++i) {
        std::cout << "i=" << i << std::endl;
        EXPECT_EQ(buffer1[i], expected_buffer1[i]);
    }

    ASSERT_EQ(state_.Counter, 1);

    result = E2E_P01Protect(&config_, &state_, buffer2);
    EXPECT_EQ(result, E2E_E_OK);
    for (size_t i = 0; i<8; ++i) {
        EXPECT_EQ(buffer2[i], expected_buffer2[i]);
    }
}
