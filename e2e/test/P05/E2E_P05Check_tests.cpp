extern "C" {
    #include "E2E_P05.h"
}

#include <gtest/gtest.h>

class P05Check : public ::testing::Test {
protected:
    static constexpr uint16_t kBufferLength = 8;
    static constexpr uint16_t kConfiguredDataLength = kBufferLength * 8;

    E2E_P05CheckStateType state_;
    E2E_P05ConfigType config_;
    E2E_P05ProtectStateType protect_state_;

    void SetUp() override {
        SetExampleConfig();
        E2E_P05CheckInit(&state_);
        E2E_P05ProtectInit(&protect_state_);
    }

    void SetExampleConfig() {
        config_.DataID = 0x1234;
        config_.Offset = 0;
        config_.DataLength = kConfiguredDataLength;
        config_.MaxDeltaCounter = 1;
    }

    void ProtectCurrentBuffer() {
        Std_ReturnType protect_result = E2E_P05Protect(&config_, &protect_state_, buffer_, kBufferLength);
        ASSERT_EQ(protect_result, E2E_E_OK);
    }

    uint8_t buffer_[kBufferLength];
};

/**
 * Test for [1] PRS_E2E_00411, PRS_E2E_00412, PRS_E2E_00416
 *
 * Test for having no new data available.
 *
 * @test E2E_P05Check sets status E2E_P05STATUS_NONEWDATA when Data is NULL
 * and Length parameter is zero
 */
TEST_F(P05Check, DataParameterNullAndLengthZeroSetsStateNONEWDATA) {
    Std_ReturnType result = E2E_P05Check(&config_, &state_, nullptr, 0);
    EXPECT_EQ(result, E2E_E_OK);
    EXPECT_EQ(state_.Status,E2E_P05STATUS_NONEWDATA);
}

/**
 * Test for [1] PRS_E2E_00411, PRS_E2E_00416
 *
 * Test mismatch between computed and received CRC
 *
 * @test E2E_P05Check sets status E2E_P05STATUS_ERROR when calculated CRC
 * and received CRC mismatches
 */
TEST_F(P05Check, MismatchedCRCSetsStateERROR) {
    Std_ReturnType result = E2E_P05Check(&config_, &state_, buffer_, kBufferLength);
    EXPECT_EQ(result, E2E_E_OK);
    EXPECT_EQ(state_.Status, E2E_P05STATUS_ERROR);
}

/**
 *
 * @test Test for multiple messages in sequence
 */
TEST_F(P05Check, ConcurrentMessagesWithSingleCounterIncrementSetsStateOK) {
    ProtectCurrentBuffer();

    Std_ReturnType result = E2E_P05Check(&config_, &state_, buffer_, kBufferLength);
    ASSERT_EQ(result, E2E_E_OK);
    ASSERT_EQ(state_.Counter, 0);

    ProtectCurrentBuffer();
    state_.Status = E2E_P05STATUS_ERROR; // Set to something else just ensure that it is reset
    result = E2E_P05Check(&config_, &state_, buffer_, kBufferLength);
    EXPECT_EQ(result, E2E_E_OK);
    EXPECT_EQ(state_.Counter, 1);
    EXPECT_EQ(state_.Status, E2E_P05STATUS_OK);

    ProtectCurrentBuffer();
    state_.Status = E2E_P05STATUS_ERROR; // Set to something else just ensure that it is reset
    result = E2E_P05Check(&config_, &state_, buffer_, kBufferLength);
    EXPECT_EQ(result, E2E_E_OK);
    EXPECT_EQ(state_.Counter, 2);
    EXPECT_EQ(state_.Status, E2E_P05STATUS_OK);
}

/**
 * @test Test for multiple messages in sequence, with counter overflow
 */
TEST_F(P05Check, ConcurrentMessagesWithCounterOverflowSetsStateOK) {
    protect_state_.Counter = 0xFF;
    state_.Counter = 0xFE;
    ProtectCurrentBuffer();
    Std_ReturnType result = E2E_P05Check(&config_, &state_, buffer_, kBufferLength);
    ASSERT_EQ(result, E2E_E_OK);
    ASSERT_EQ(state_.Counter, 0xFF);
    ASSERT_EQ(protect_state_.Counter, 0);

    ProtectCurrentBuffer();
    state_.Status = E2E_P05STATUS_ERROR; // Set to something else just ensure that it is reset
    result = E2E_P05Check(&config_, &state_, buffer_, kBufferLength);
    EXPECT_EQ(result, E2E_E_OK);
    EXPECT_EQ(state_.Counter, 0);
    EXPECT_EQ(state_.Status, E2E_P05STATUS_OK);
}
