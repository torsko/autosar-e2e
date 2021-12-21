/**
 * @file E2E_P05Protect_tests.cpp
 * @author Tomas Wester (torsko@gmail.com)
 * @brief
 * @version 0.1
 * @date 2021-11-23
 *
 * References
 *
 * [1] https://www.autosar.org/fileadmin/user_upload/standards/foundation/20-11/AUTOSAR_PRS_E2EProtocol.pdf
 *
 */

extern "C" {
    #include "E2E_P05.h"
}

#include <gtest/gtest.h>

/**
 * Test fixture for P05Protect suite of test cases
 */
class P05Protect : public ::testing::Test {
protected:
    static constexpr size_t kBufferLength{8};
    static constexpr size_t kConfigDataLength{kBufferLength*8};

    void SetUp() override {
        SetExampleConfig();
        SetExampleState();
        memset(buffer_, 0, kBufferLength);
    }

    /**
     * This configuration is taken from [1] 6.6.5 E2E Profile 5 Protocol Examples
     * with the exception of the DataLength being (8*8) instead of (8*3) to accommodate
     * some data payload and not just the E2E header.
     */
    void SetExampleConfig() {
        config_.Offset = 0x0000;
        config_.DataID = 0x1234;
        config_.DataLength = kConfigDataLength;
        config_.MaxDeltaCounter = 1;
    }

    void SetExampleState() {
        state_.Counter = 0;
    }

    uint8_t buffer_[kBufferLength];
    E2E_P05ConfigType config_;
    E2E_P05ProtectStateType state_;

};

/**
 * Test for [1] PRS_E2E_00405
 *
 * The step ”Write Counter” in E2E_P05Protect()
 * shall behave as shown in Figure 6.52.
 *
 * Figure 6.52 can be found in [1]
 *
 * @test E2E_P05Protect writes the State->Counter in the Data at
 * index (Config->Offset / 8U) + 2
 */
TEST_F(P05Protect, WriteCounter) {
    state_.Counter = 0xAB;
    Std_ReturnType result = E2E_P05Protect(&config_, &state_, buffer_, kBufferLength);
    EXPECT_EQ(result, E2E_E_OK);
    EXPECT_EQ(buffer_[(config_.Offset / 8U) + 2], 0xAB);
}


/**
 * Test 1/2 for [1] PRS_E2E_00406, PRS_E2E_00407
 *
 * PRS_E2E_00406:
 * The step ”Compute CRC” in E2E_P05Protect() and in
 * E2E_P05Check shall behave as shown in Figure 6.53.
 *
 * PRS_E2E_00407:
 * The step ”Write CRC” in E2E_P05Protect() and
 * E2E_P05Forward() shall behave as shown in Figure 6.54.
 *
 * To have known inputs/outputs, we use the example in [1] Table 6.47
 *
 * @test E2E_P05Protect computes the CRC and stores it, little endian, in the first two bytes
 * when offset is zero
 */
TEST_F(P05Protect, ComputeCRCAndWriteCRCWithOffsetZero) {
    const uint16_t expectedCRC = 0xca1c;
    Std_ReturnType result = E2E_P05Protect(&config_, &state_, buffer_, kBufferLength);

    EXPECT_EQ(result, E2E_E_OK);
    EXPECT_EQ(buffer_[0], 0x1c);
    EXPECT_EQ(buffer_[1], 0xca);
}

/**
 * Test 2/2 for [1] PRS_E2E_00406, PRS_E2E_00407
 *
 * PRS_E2E_00406:
 * The step ”Compute CRC” in E2E_P05Protect() and in
 * E2E_P05Check shall behave as shown in Figure 6.53.
 *
 * PRS_E2E_00407:
 * The step ”Write CRC” in E2E_P05Protect() and
 * E2E_P05Forward() shall behave as shown in Figure 6.54.
 *
 * To have known inputs/outputs, we use the example in [1] Table 6.47
 * but we use a non-zero offset to put the E2E-header in the middle of the
 * byte array instead, the CRC is the same regardless
 *
 * @test E2E_P05Protect computes the CRC and stores it, little endian, in the two bytes
 * starting at configured, non-zero, offset
 */
TEST_F(P05Protect, ComputeCRCAndWriteCRCWithOffsetNonZero) {
    const uint16_t expectedCRC = 0xca1c;
    config_.Offset = 3*8;
    Std_ReturnType result = E2E_P05Protect(&config_, &state_, buffer_, kBufferLength);

    EXPECT_EQ(result, E2E_E_OK);
    EXPECT_EQ(buffer_[3], 0x1c);
    EXPECT_EQ(buffer_[4], 0xca);
}

/**
 * Test 1/2 for [1] PRS_E2E_00409
 *
 * PRS_E2E_00409:
 * The step ”Increment Counter” in E2E_P05Protect() and
 * E2E_P05Forward() shall behave as shown in Figure 6.55.
 *
 * @test E2E_P05Protect increments the counter when input is valid
 */
TEST_F(P05Protect, IncrementCounter) {
    state_.Counter = 0xab;
    Std_ReturnType result = E2E_P05Protect(&config_, &state_, buffer_, kBufferLength);

    EXPECT_EQ(result, E2E_E_OK);
    EXPECT_EQ(state_.Counter, 0xac);
}

/**
 * Test 2/2 for [1] PRS_E2E_00409
 *
 * PRS_E2E_00409:
 * The step ”Increment Counter” in E2E_P05Protect() and
 * E2E_P05Forward() shall behave as shown in Figure 6.55.
 *
 * @test E2E_P05Protect restarts the counter at zero after 0xff
 */
TEST_F(P05Protect, IncrementCounterResetsAfterMaxUint8) {
    state_.Counter = 0xff;
    Std_ReturnType result = E2E_P05Protect(&config_, &state_, buffer_, kBufferLength);

    EXPECT_EQ(result, E2E_E_OK);
    EXPECT_EQ(state_.Counter, 0x0);
}


/**
 * Test 1/4 for [1] PRS_E2E_00403
 *
 * PRS_E2E_00403:
 * The function E2E_P05Protect() shall have
 * the overall behavior as shown in Figure 6.49.
 *
 * @test E2E_P05Protect does not modify the data buffer on wrong input
 */
TEST_F(P05Protect, NoBufferModificationOnWrongInput) {

    for (int i=0; i<kBufferLength; ++i) {
        buffer_[i] = 0xAA;
    }

    Std_ReturnType result = E2E_P05Protect(&config_, &state_, buffer_, kBufferLength+1);

    ASSERT_EQ(result, E2E_E_INPUTERR_WRONG);
    for (int i=0; i<kBufferLength; ++i) {
        EXPECT_EQ(buffer_[i], 0xAA);
    }
}

/**
 * Test 2/4 for [1] PRS_E2E_00403
 *
 * PRS_E2E_00403:
 * The function E2E_P05Protect() shall have
 * the overall behavior as shown in Figure 6.49.
 *
 * @test E2E_P05Protect does not modify the data buffer on NULL input
 */
TEST_F(P05Protect, NoBufferModificationOnNULLInput) {

    for (int i=0; i<kBufferLength; ++i) {
        buffer_[i] = 0xAA;
    }

    Std_ReturnType result = E2E_P05Protect(&config_, nullptr, buffer_, kBufferLength+1);

    ASSERT_EQ(result, E2E_E_INPUTERR_NULL);
    for (int i=0; i<kBufferLength; ++i) {
        EXPECT_EQ(buffer_[i], 0xAA);
    }
}

/**
 * Test 3/4 for [1] PRS_E2E_00403
 *
 * PRS_E2E_00403:
 * The function E2E_P05Protect() shall have
 * the overall behavior as shown in Figure 6.49.
 *
 * @test E2E_P05Protect does not increment the state counter on wrong input
 */
TEST_F(P05Protect, NoCounterModificationOnWrongInput) {
    state_.Counter = 0xAA;
    Std_ReturnType result = E2E_P05Protect(&config_, &state_, buffer_, kBufferLength+1);

    ASSERT_EQ(result, E2E_E_INPUTERR_WRONG);
    EXPECT_EQ(state_.Counter, 0xAA);
}

/**
 * Test 4/4 for [1] PRS_E2E_00403
 *
 * PRS_E2E_00403:
 * The function E2E_P05Protect() shall have
 * the overall behavior as shown in Figure 6.49.
 *
 * @test E2E_P05Protect does not increment the state counter on NULL input
 */
TEST_F(P05Protect, NoCounterModificationOnNULLInput) {
    state_.Counter = 0xAA;
    Std_ReturnType result = E2E_P05Protect(&config_, nullptr, buffer_, kBufferLength+1);

    ASSERT_EQ(result, E2E_E_INPUTERR_NULL);
    EXPECT_EQ(state_.Counter, 0xAA);
}


/**
 * Test 1/2 for [1] 6.6.5 E2E Profile 5 Protocol Examples
 *
 * Example data taken from Table 6.47: E2E Profile 5 example short
 *
 * @test E2E_P05Protect creates expected results from [1] Table 6.47
 */
TEST_F(P05Protect, ProtocolExample1) {
    uint8_t expected_buffer[8] = { 0x1c, 0xca, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    uint8_t buffer[8];
    memset(buffer, 0, 8);

    Std_ReturnType result = E2E_P05Protect(&config_, &state_, buffer, 8);

    for (int i = 0; i<8; ++i) {
        EXPECT_EQ(buffer[i], expected_buffer[i]);
    }
}

/**
 * Test 2/2 for [1] 6.6.5 E2E Profile 5 Protocol Examples
 *
 * Example data taken from Table 6.48: E2E Profile 5 example short
 *
 * @test E2E_P05Protect creates expected results from [1] Table 6.48
 */
TEST_F(P05Protect, ProtocolExample2) {
    uint8_t expected_buffer[16] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x28, 0x91, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
    uint8_t buffer[16];
    memset(buffer, 0, 16);
    config_.Offset = 64;
    config_.DataLength = 128;

    Std_ReturnType result = E2E_P05Protect(&config_, &state_, buffer, 16);

    for (int i = 0; i<16; ++i) {
        EXPECT_EQ(buffer[i], expected_buffer[i]);
    }
}

/**
 * Test for [1] PRS_E2E_00397
 *
 * @test Counter shall be initialized with 0 and incremented by 1 for every
 * subsequent protect request. When it reaches the maximum value it shall
 * be restarted at 0.
 */
TEST_F(P05Protect, CounterRestart) {
    E2E_P05ProtectStateType state;
    E2E_P05ProtectInit(&state);

    ASSERT_EQ(state.Counter, 0);
    Std_ReturnType result;
    for (int i=1; i<=255; ++i) {
        result = E2E_P05Protect(&config_, &state, buffer_, kBufferLength);
        ASSERT_EQ(result, E2E_E_OK);
        ASSERT_EQ(state.Counter, i);
    }

    result = E2E_P05Protect(&config_, &state, buffer_, kBufferLength);
    EXPECT_EQ(result, E2E_E_OK);
    EXPECT_EQ(state.Counter, 0);
}
