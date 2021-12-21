/**
 * @file E2E_P05Check_InputValidation_tests.cpp
 * @author Tomas Wester (torsko@gmail.com)
 * @brief
 * @version 0.1
 * @date 2021-11-28
 *
 * References
 *
 * [1] https://www.autosar.org/fileadmin/user_upload/standards/foundation/20-11/AUTOSAR_PRS_E2EProtocol.pdf
 */

extern "C" {
    #include "E2E_P05.h"
}

#include <gtest/gtest.h>

class P05Check_InputValidation : public ::testing::Test {
protected:
    static constexpr uint16_t kBufferLength = 8;
    static constexpr uint16_t kConfigDataLength = kBufferLength * 8U;

    void SetUp() override {
        config_.DataID = 0x1234;
        config_.Offset = 0;
        config_.DataLength = kConfigDataLength;
        config_.MaxDeltaCounter = 1;

        memset(buffer_, 0, kBufferLength);
    }

    E2E_P05ConfigType config_;
    E2E_P05CheckStateType state_;

    uint8_t buffer_[kBufferLength];
};

/**
 * Test 1/? for [1] PRS_E2E_00412
 *
 * The step ”Verify inputs of the check function” in
 * E2E_P05Check() shall behave as shown in Figure 6.61.
 *
 *
 * @test E2E_P05Check returns E2E_E_INPUTERR_NULL when provided Config
 * parameter is NULL
 */
TEST_F(P05Check_InputValidation, NullConfigReturnsINPUTERR_NULL) {
    Std_ReturnType result = E2E_P05Check(nullptr, &state_, buffer_, kBufferLength);
    EXPECT_EQ(result, E2E_E_INPUTERR_NULL);
}

/**
 * Test 2/? for [1] PRS_E2E_00412
 *
 * The step ”Verify inputs of the check function” in
 * E2E_P05Check() shall behave as shown in Figure 6.61.
 *
 *
 * @test E2E_P05Check returns E2E_E_INPUTERR_NULL when provided State
 * parameter is NULL
 */

TEST_F(P05Check_InputValidation, NullStateReturnsINPUTERR_NULL) {
    Std_ReturnType result = E2E_P05Check(&config_, nullptr, buffer_, kBufferLength);
    EXPECT_EQ(result, E2E_E_INPUTERR_NULL);
}

/**
 * Test 3/? for [1] PRS_E2E_00412
 *
 * The step ”Verify inputs of the check function” in
 * E2E_P05Check() shall behave as shown in Figure 6.61.
 *
 *
 * @test E2E_P05Check returns E2E_E_INPUTERR_WRONG when provided Data is NULL
 * and Length is not zero
 */

TEST_F(P05Check_InputValidation, DataIsNullLengthIsNotZeroReturnsINPUTERR_WRONG) {
    Std_ReturnType result = E2E_P05Check(&config_, &state_, NULL, kBufferLength);
    EXPECT_EQ(result, E2E_E_INPUTERR_WRONG);
}

/**
 * Test 4/? for [1] PRS_E2E_00412
 *
 * The step ”Verify inputs of the check function” in
 * E2E_P05Check() shall behave as shown in Figure 6.61.
 *
 *
 * @test E2E_P05Check returns E2E_E_INPUTERR_WRONG when provided Data is not NULL
 * and Length is zero
 */

TEST_F(P05Check_InputValidation, DataIsNotNullLengthIsZeroReturnsINPUTERR_WRONG) {
    Std_ReturnType result = E2E_P05Check(&config_, &state_, buffer_, 0);
    EXPECT_EQ(result, E2E_E_INPUTERR_WRONG);
}

/**
 * Test 5/? for [1] PRS_E2E_00412
 *
 * The step ”Verify inputs of the check function” in
 * E2E_P05Check() shall behave as shown in Figure 6.61.
 *
 *
 * @test E2E_P05Check returns E2E_E_INPUTERR_WRONG when Length parameter and
 * Configured DataLength does not match
 */
TEST_F(P05Check_InputValidation, LengthParameterAndConfigDataLengthMismatchReturnsINPUTERR_WRONG) {
    Std_ReturnType result = E2E_P05Check(&config_, &state_, buffer_, kBufferLength + 1);
    EXPECT_EQ(result, E2E_E_INPUTERR_WRONG);
}

// TEST_F(P05Check_InputValidation, DataLengthMustAtLeastHoldCrcAndCounter) {
//     config_.DataLength = 0;
//     Std_ReturnType result = E2E_P05Check(&config_, &state_, buffer_);
//     EXPECT_EQ(result, E2E_E_INPUTERR_WRONG);

//     config_.DataLength = 8;
//     result = E2E_P05Check(&config_, &state_, buffer_);
//     EXPECT_EQ(result, E2E_E_INPUTERR_WRONG);

//     config_.DataLength = 16;
//     result = E2E_P05Check(&config_, &state_, buffer_);
//     EXPECT_EQ(result, E2E_E_OK);
// }


// TEST_F(P05Check_InputValidation, DataLengthMustBeMultipleOfEight) {
//     state_.NewDataAvailable = false; // To avoid triggering CRC-calculations
//     config_.DataLength = 15;

//     Std_ReturnType result = E2E_P05Check(&config_, &state_, buffer_);
//     EXPECT_EQ(result, E2E_E_INPUTERR_WRONG);

//     config_.DataLength = 16;
//     result = E2E_P05Check(&config_, &state_, buffer_);
//     EXPECT_EQ(result, E2E_E_OK);

//     config_.DataLength = 17;
//     result = E2E_P05Check(&config_, &state_, buffer_);
//     EXPECT_EQ(result, E2E_E_INPUTERR_WRONG);

//     config_.DataLength = 23;
//     result = E2E_P05Check(&config_, &state_, buffer_);
//     EXPECT_EQ(result, E2E_E_INPUTERR_WRONG);

//     config_.DataLength = 24;
//     result = E2E_P05Check(&config_, &state_, buffer_);
//     EXPECT_EQ(result, E2E_E_OK);

//     config_.DataLength = 25;
//     result = E2E_P05Check(&config_, &state_, buffer_);
//     EXPECT_EQ(result, E2E_E_INPUTERR_WRONG);
// }
