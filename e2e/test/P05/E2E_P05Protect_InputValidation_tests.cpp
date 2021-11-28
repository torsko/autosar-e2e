/**
 * @file E2E_P05Protect_InputValidation_tests.cpp
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
 * Test fixture for P05Protect_InputValidation suite of tests
 */
class P05Protect_InputValidation : public ::testing::Test {
protected:
    static constexpr size_t kBufferLength{8};
    static constexpr size_t kConfigDataLength{kBufferLength*8};

    void SetUp() override {
        config_.Offset = 0;
        config_.MaxDeltaCounter = 1;
        config_.DataID = 0;
        config_.DataLength = kConfigDataLength;
    }

    E2E_P05ConfigType config_;
    E2E_P05ProtectStateType state_;
    uint8_t buffer_[kBufferLength];
};

/**
 * Test 1/4 for [1] PRS_E2E_00404
 *
 * The step ”Verify inputs of the protect function” in
 * E2E_P05Protect() shall behave as shown in Figure 6.50.
 *
 * Figure 6.50 can be found in [1]
 *
 * @test E2E_P05Protect returns E2E_E_INPUTERR_NULL when
 * the pointer to the Config struct is NULL
 */
TEST_F(P05Protect_InputValidation, ConfigParameterIsNull) {
    Std_ReturnType result = E2E_P05Protect(nullptr, &state_, buffer_, kBufferLength);
    EXPECT_EQ(result, E2E_E_INPUTERR_NULL);
}

/**
 * Test 2/4 for [1] PRS_E2E_00404
 *
 * The step ”Verify inputs of the protect function” in
 * E2E_P05Protect() shall behave as shown in Figure 6.50.
 *
 * Figure 6.50 can be found in [1]
 *
 * @test E2E_P05Protect returns E2E_E_INPUTERR_NULL when
 * the pointer to the State struct is NULL
 */
TEST_F(P05Protect_InputValidation, StateParameterIsNull) {
    Std_ReturnType result = E2E_P05Protect(&config_, nullptr, buffer_, kBufferLength);
    EXPECT_EQ(result, E2E_E_INPUTERR_NULL);
}

/**
 * Test 3/4 for [1] PRS_E2E_00404
 *
 * The step ”Verify inputs of the protect function” in
 * E2E_P05Protect() shall behave as shown in Figure 6.50.
 *
 * Figure 6.50 can be found in [1]
 *
 * @test E2E_P05Protect returns E2E_E_INPUTERR_NULL when
 * the pointer to Data is NULL
 */
TEST_F(P05Protect_InputValidation, DataParameterIsNull) {
    Std_ReturnType result = E2E_P05Protect(&config_, &state_, nullptr, 0);
    EXPECT_EQ(result, E2E_E_INPUTERR_NULL);
}


/**
 * Test 4/4 for [1] PRS_E2E_00404
 *
 * The step ”Verify inputs of the protect function” in
 * E2E_P05Protect() shall behave as shown in Figure 6.50.
 *
 * Figure 6.50 can be found in [1]
 *
 * @test E2E_P05Protect returns E2E_E_INPUTERR_WRONG when
 * the provided Length parameter does not match the data length in configuration
 */
TEST_F(P05Protect_InputValidation, LengthParameterAndConfigMismatch) {
    const uint16_t invalidLengthParameter = kBufferLength - 1;
    ASSERT_NE(config_.DataLength/8, invalidLengthParameter);
    Std_ReturnType result = E2E_P05Protect(&config_, &state_, buffer_, invalidLengthParameter);

    EXPECT_EQ(result, E2E_E_INPUTERR_WRONG);
}



/**
 * Test 1/4 for [1] PRS_E2E_00654
 *
 * The E2E_P05Protect, E2E_P05Forward and E2E_P05Check
 * functions ’Config’ shall have the following members defined in (see Table 6.44).
 *
 * From Table 6.44 regarding Offset:
 * "Bit offset of the first bit of the E2E header
 * from the beginning of the Data (bit num-
 * bering: bit 0 is the least important). The
 * offset shall be a multiple of 8 and 0 <=
 * Offset <= DataLength-(3*8)."
 */
TEST_F(P05Protect_InputValidation, ConfigOffsetByteAlignment) {
    config_.Offset = 0;
    Std_ReturnType result = E2E_P05Protect(&config_, &state_, buffer_, kBufferLength);
    EXPECT_EQ(result, E2E_E_OK);

    config_.Offset = 1;
    result = E2E_P05Protect(&config_, &state_, buffer_, kBufferLength);
    EXPECT_EQ(result, E2E_E_INPUTERR_WRONG);

    config_.Offset = 7;
    result = E2E_P05Protect(&config_, &state_, buffer_, kBufferLength);
    EXPECT_EQ(result, E2E_E_INPUTERR_WRONG);

    config_.Offset = 8;
    result = E2E_P05Protect(&config_, &state_, buffer_, kBufferLength);
    EXPECT_EQ(result, E2E_E_OK);

    config_.Offset = 9;
    result = E2E_P05Protect(&config_, &state_, buffer_, kBufferLength);
    EXPECT_EQ(result, E2E_E_INPUTERR_WRONG);
}

/**
 * Test 2/4 for [1] PRS_E2E_00654
 *
 * The E2E_P05Protect, E2E_P05Forward and E2E_P05Check
 * functions ’Config’ shall have the following members defined in (see Table 6.44).
 *
 * From Table 6.44 regarding Offset:
 * "Bit offset of the first bit of the E2E header
 * from the beginning of the Data (bit num-
 * bering: bit 0 is the least important). The
 * offset shall be a multiple of 8 and 0 <=
 * Offset <= DataLength-(3*8)."
 *
 * @test E2E_P05Protect returns E2E_E_INPUTERR_WRONG when the configured
 * offset is outside the configured DataLength
 */
TEST_F(P05Protect_InputValidation, ConfigOffsetRangeCheck) {
    // The test will start with an offset equal to data length and then
    // work its way back in the buffer until it reaches a known valid state

    config_.Offset = config_.DataLength;
    Std_ReturnType result = E2E_P05Protect(&config_, &state_, buffer_, kBufferLength);
    EXPECT_EQ(result, E2E_E_INPUTERR_WRONG);

    config_.Offset = config_.DataLength - (1*8);
    result = E2E_P05Protect(&config_, &state_, buffer_, kBufferLength);
    EXPECT_EQ(result, E2E_E_INPUTERR_WRONG);

    config_.Offset = config_.DataLength - (2*8);
    result = E2E_P05Protect(&config_, &state_, buffer_, kBufferLength);
    EXPECT_EQ(result, E2E_E_INPUTERR_WRONG);

    config_.Offset = config_.DataLength - (3*8);
    result = E2E_P05Protect(&config_, &state_, buffer_, kBufferLength);
    EXPECT_EQ(result, E2E_E_OK);
}

/**
 * Test 3/4 for [1] PRS_E2E_00654
 *
 * The E2E_P05Protect, E2E_P05Forward and E2E_P05Check
 * functions ’Config’ shall have the following members defined in (see Table 6.44).
 *
 * From Table 6.44 regarding DataLength:
 * "Length of Data, in bits. The value shall be
 * <= 4096*8 (4kB) and shall be >= 3*8"
 *
 * @test E2E_P05Protect returns E2E_E_INPUTERR_WRONG when the configured
 * DataLength is less than 3*8
 */
TEST_F(P05Protect_InputValidation, ConfigDataLengthMinimumValue) {
    // The test will start with a zero data length which increases
    // a single byte at a time until it reaches the valid minimum value
    // Note that the provided length in the E2E_P05Protect is no longer the
    // constant kBufferLength since this would trigger another check (Length != Config->DataLength)

    config_.DataLength = 0;
    Std_ReturnType result = E2E_P05Protect(&config_, &state_, buffer_, config_.DataLength / 8U);
    EXPECT_EQ(result, E2E_E_INPUTERR_WRONG);

    config_.DataLength = (1*8);
    result = E2E_P05Protect(&config_, &state_, buffer_, config_.DataLength / 8U);
    EXPECT_EQ(result, E2E_E_INPUTERR_WRONG);

    config_.DataLength = (2*8);
    result = E2E_P05Protect(&config_, &state_, buffer_, config_.DataLength / 8U);
    EXPECT_EQ(result, E2E_E_INPUTERR_WRONG);

    config_.DataLength = (3*8);
    result = E2E_P05Protect(&config_, &state_, buffer_, config_.DataLength / 8U);
    EXPECT_EQ(result, E2E_E_OK);
}

/**
 * Test 4/4 for [1] PRS_E2E_00654
 *
 * The E2E_P05Protect, E2E_P05Forward and E2E_P05Check
 * functions ’Config’ shall have the following members defined in (see Table 6.44).
 *
 * From Table 6.44 regarding DataLength:
 * "Length of Data, in bits. The value shall be
 * <= 4096*8 (4kB) and shall be >= 3*8"
 *
 * @test E2E_P05Protect returns E2E_E_INPUTERR_WRONG when the configured
 * DataLength is greater than 4096*8
 */
TEST_F(P05Protect_InputValidation, ConfigDataLengthMaximumValue) {
    // The test will use the boundary value +- 1
    // Note that the provided length in the E2E_P05Protect is no longer the
    // constant kBufferLength since this would trigger another check (Length != Config->DataLength)

    config_.DataLength = (4096-1)*8;
    Std_ReturnType result = E2E_P05Protect(&config_, &state_, buffer_, config_.DataLength / 8U);
    EXPECT_EQ(result, E2E_E_OK);

    config_.DataLength = 4096*8;
    result = E2E_P05Protect(&config_, &state_, buffer_, config_.DataLength / 8U);
    EXPECT_EQ(result, E2E_E_OK);

    config_.DataLength = (4096+1)*8;
    result = E2E_P05Protect(&config_, &state_, buffer_, config_.DataLength / 8U);
    EXPECT_EQ(result, E2E_E_INPUTERR_WRONG);
}
