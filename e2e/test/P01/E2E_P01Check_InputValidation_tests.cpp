extern "C" {
    #include "E2E_P01.h"
}

#include <gtest/gtest.h>

class P01Check_InputValidation : public ::testing::Test {
protected:
    void SetUp() override {
        config_.CounterOffset = 0;
        config_.CRCOffset = 0;
        config_.DataID = 0;
        config_.DataIDMode = E2E_P01_DATAID_BOTH;
        config_.DataIDNibbleOffset = 0;
        config_.DataLength = 64;

        memset(buffer_, 0, 5);
    }

    E2E_P01ConfigType config_;
    E2E_P01CheckStateType state_;

    uint8_t buffer_[5];
};

TEST_F(P01Check_InputValidation, NullConfigReturnsINPUTERR_NULL) {
    Std_ReturnType result = E2E_P01Check(nullptr, &state_, buffer_);

    EXPECT_EQ(result, E2E_E_INPUTERR_NULL);
}

TEST_F(P01Check_InputValidation, NullStateReturnsINPUTERR_NULL) {
    Std_ReturnType result = E2E_P01Check(&config_, nullptr, buffer_);

    EXPECT_EQ(result, E2E_E_INPUTERR_NULL);
}

TEST_F(P01Check_InputValidation, NullDataReturnsINPUTERR_NULL) {
    Std_ReturnType result = E2E_P01Check(&config_, &state_, nullptr);

    EXPECT_EQ(result, E2E_E_INPUTERR_NULL);
}

/**
 * Bit offset of Counter in MSB first order. CounterOffset shall be a multiple
 * of 4. In variants 1A, 1B, and 1C, CounterOffset is 8.
 */
TEST_F(P01Check_InputValidation, CounterOffsetNotNibbleAlignedReturnsINPUTERR_WRONG) {
    config_.CounterOffset = 0;
    Std_ReturnType result = E2E_P01Check(&config_, &state_, buffer_);
    EXPECT_EQ(result, E2E_E_OK);

    config_.CounterOffset = 1;
    result = E2E_P01Check(&config_, &state_, buffer_);
    EXPECT_EQ(result, E2E_E_INPUTERR_WRONG);

    config_.CounterOffset = 3;
    result = E2E_P01Check(&config_, &state_, buffer_);
    EXPECT_EQ(result, E2E_E_INPUTERR_WRONG);

    config_.CounterOffset = 4;
    result = E2E_P01Check(&config_, &state_, buffer_);
    EXPECT_EQ(result, E2E_E_OK);

    config_.CounterOffset = 5;
    result = E2E_P01Check(&config_, &state_, buffer_);
    EXPECT_EQ(result, E2E_E_INPUTERR_WRONG);
}

/**
 * Bit offset of CRC (i.e. since *Data) in MSB first order. The offset shall be
 * a multiple of 8. In variants 1A, 1B, and 1C, CRCOffset is 0.
 */
TEST_F(P01Check_InputValidation, CRCOffsetNotByteAlignedReturnsINPUTERR_WRONG) {
    config_.CRCOffset = 0;
    Std_ReturnType result = E2E_P01Check(&config_, &state_, buffer_);
    EXPECT_EQ(result, E2E_E_OK);

    config_.CRCOffset = 1;
    result = E2E_P01Check(&config_, &state_, buffer_);
    EXPECT_EQ(result, E2E_E_INPUTERR_WRONG);

    config_.CRCOffset = 7;
    result = E2E_P01Check(&config_, &state_, buffer_);
    EXPECT_EQ(result, E2E_E_INPUTERR_WRONG);

    config_.CRCOffset = 8;
    result = E2E_P01Check(&config_, &state_, buffer_);
    EXPECT_EQ(result, E2E_E_OK);

    config_.CRCOffset = 9;
    result = E2E_P01Check(&config_, &state_, buffer_);
    EXPECT_EQ(result, E2E_E_INPUTERR_WRONG);
}

TEST_F(P01Check_InputValidation, DataIdModeOutsideValidRangeReturnsINPUTERR_WRONG) {
    config_.DataIDMode = E2E_P01_DATAID_BOTH;
    Std_ReturnType result = E2E_P01Check(&config_, &state_, buffer_);
    EXPECT_EQ(result, E2E_E_OK);

    config_.DataIDMode = E2E_P01_DATAID_ALT;
    result = E2E_P01Check(&config_, &state_, buffer_);
    EXPECT_EQ(result, E2E_E_OK);

    config_.DataIDMode = E2E_P01_DATAID_LOW;
    result = E2E_P01Check(&config_, &state_, buffer_);
    EXPECT_EQ(result, E2E_E_OK);

    config_.DataIDMode = E2E_P01_DATAID_NIBBLE;
    result = E2E_P01Check(&config_, &state_, buffer_);
    EXPECT_EQ(result, E2E_E_OK);

    config_.DataIDMode = (E2E_P01DataIDMode)0xFF;
    result = E2E_P01Check(&config_, &state_, buffer_);
    EXPECT_EQ(result, E2E_E_INPUTERR_WRONG);
}

/**
 *
 * Bit offset of the low nibble of the high byte of Data ID. This parameter is
 * used by E2E Library only if DataIDMode = E2E_P01_DATAID_NIBBLE
 * (otherwise it is ignored by E2E Library).
 *
 * For DataIDMode different than E2E_P01_DATAID_NIBBLE, DataIDNibbleOffset
 * shall be initialized to 0 (even if it is ignored by E2E Library)
 *
 */
TEST_F(P01Check_InputValidation, DataIdNibbleOffsetMustBeZeroUnlessDATAID_NIBBLE) {
    config_.DataIDNibbleOffset = 1;
    config_.DataIDMode = E2E_P01_DATAID_BOTH;
    Std_ReturnType result = E2E_P01Check(&config_, &state_, buffer_);
    EXPECT_EQ(result, E2E_E_INPUTERR_WRONG);

    config_.DataIDNibbleOffset = 0;
    config_.DataIDMode = E2E_P01_DATAID_BOTH;
    result = E2E_P01Check(&config_, &state_, buffer_);
    EXPECT_EQ(result, E2E_E_OK);

    config_.DataIDMode = E2E_P01_DATAID_ALT;
    result = E2E_P01Check(&config_, &state_, buffer_);
    EXPECT_EQ(result, E2E_E_OK);

    config_.DataIDMode = E2E_P01_DATAID_ALT;
    result = E2E_P01Check(&config_, &state_, buffer_);
    EXPECT_EQ(result, E2E_E_OK);

    config_.DataIDMode = E2E_P01_DATAID_LOW;
    result = E2E_P01Check(&config_, &state_, buffer_);
    EXPECT_EQ(result, E2E_E_OK);

    config_.DataIDMode = E2E_P01_DATAID_NIBBLE;
    result = E2E_P01Check(&config_, &state_, buffer_);
    EXPECT_EQ(result, E2E_E_OK);

    config_.DataIDMode = (E2E_P01DataIDMode)0xFF;
    result = E2E_P01Check(&config_, &state_, buffer_);
    EXPECT_EQ(result, E2E_E_INPUTERR_WRONG);
}

TEST_F(P01Check_InputValidation, DataIdNibbleOffsetMustByteOrNibbleAligned) {
    config_.DataIDMode = E2E_P01_DATAID_NIBBLE;

    config_.DataIDNibbleOffset = 0;
    Std_ReturnType result = E2E_P01Check(&config_, &state_, buffer_);
    EXPECT_EQ(result, E2E_E_OK);

    config_.DataIDNibbleOffset = 3;
    result = E2E_P01Check(&config_, &state_, buffer_);
    EXPECT_EQ(result, E2E_E_INPUTERR_WRONG);

    config_.DataIDNibbleOffset = 4;
    result = E2E_P01Check(&config_, &state_, buffer_);
    EXPECT_EQ(result, E2E_E_OK);

    config_.DataIDNibbleOffset = 5;
    result = E2E_P01Check(&config_, &state_, buffer_);
    EXPECT_EQ(result, E2E_E_INPUTERR_WRONG);

    config_.DataIDNibbleOffset = 7;
    result = E2E_P01Check(&config_, &state_, buffer_);
    EXPECT_EQ(result, E2E_E_INPUTERR_WRONG);

    config_.DataIDNibbleOffset = 8;
    result = E2E_P01Check(&config_, &state_, buffer_);
    EXPECT_EQ(result, E2E_E_OK);

    config_.DataIDNibbleOffset = 9;
    result = E2E_P01Check(&config_, &state_, buffer_);
    EXPECT_EQ(result, E2E_E_INPUTERR_WRONG);
}

TEST_F(P01Check_InputValidation, DataLengthMustAtLeastHoldCrcAndCounter) {
    config_.DataLength = 0;
    Std_ReturnType result = E2E_P01Check(&config_, &state_, buffer_);
    EXPECT_EQ(result, E2E_E_INPUTERR_WRONG);

    config_.DataLength = 8;
    result = E2E_P01Check(&config_, &state_, buffer_);
    EXPECT_EQ(result, E2E_E_INPUTERR_WRONG);

    config_.DataLength = 16;
    result = E2E_P01Check(&config_, &state_, buffer_);
    EXPECT_EQ(result, E2E_E_OK);
}


TEST_F(P01Check_InputValidation, DataLengthMustBeMultipleOfEight) {
    state_.NewDataAvailable = false; // To avoid triggering CRC-calculations
    config_.DataLength = 15;

    Std_ReturnType result = E2E_P01Check(&config_, &state_, buffer_);
    EXPECT_EQ(result, E2E_E_INPUTERR_WRONG);

    config_.DataLength = 16;
    result = E2E_P01Check(&config_, &state_, buffer_);
    EXPECT_EQ(result, E2E_E_OK);

    config_.DataLength = 17;
    result = E2E_P01Check(&config_, &state_, buffer_);
    EXPECT_EQ(result, E2E_E_INPUTERR_WRONG);

    config_.DataLength = 23;
    result = E2E_P01Check(&config_, &state_, buffer_);
    EXPECT_EQ(result, E2E_E_INPUTERR_WRONG);

    config_.DataLength = 24;
    result = E2E_P01Check(&config_, &state_, buffer_);
    EXPECT_EQ(result, E2E_E_OK);

    config_.DataLength = 25;
    result = E2E_P01Check(&config_, &state_, buffer_);
    EXPECT_EQ(result, E2E_E_INPUTERR_WRONG);
}
