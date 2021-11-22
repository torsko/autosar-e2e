extern "C" {
    #include "E2E_P01.h"
}

#include <gtest/gtest.h>

class P01Check : public ::testing::Test {
protected:
    E2E_P01CheckStateType state_;
    E2E_P01ConfigType config_;
    E2E_P01ProtectStateType protect_state_;

    void SetUp() override {
        SetExampleConfig();
        E2E_P01CheckInit(&state_);
        E2E_P01ProtectInit(&protect_state_);
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

    void ProtectCurrentBuffer() {
        Std_ReturnType protect_result = E2E_P01Protect(&config_, &protect_state_, buffer_);
        ASSERT_EQ(protect_result, E2E_E_OK);
    }

    uint8_t buffer_[8];
};

TEST_F(P01Check, NoNewDataAvailableSetsStateNONEWDATA) {
    state_.NewDataAvailable = false;
    Std_ReturnType result = E2E_P01Check(&config_, &state_, buffer_);

    EXPECT_EQ(result, E2E_E_OK);
    EXPECT_EQ(state_.Status, E2E_P01STATUS_NONEWDATA);
}

TEST_F(P01Check, NoNewDataAvailableIncrementsNoNewOrRepeatedDataCounter) {
    state_.NewDataAvailable = false;
    state_.NoNewOrRepeatedDataCounter = 0;

    Std_ReturnType result = E2E_P01Check(&config_, &state_, buffer_);

    EXPECT_EQ(result, E2E_E_OK);
    EXPECT_EQ(state_.NoNewOrRepeatedDataCounter, 1);
}

TEST_F(P01Check, NoNewDataAvailableIncrementsNoNewOrRepeatedDataCounterCappedAtMaxValue) {
    const int iterations = 20;
    const uint8_t expectedCounterValue = 14;
    state_.NewDataAvailable = false;
    state_.NoNewOrRepeatedDataCounter = 0;
    Std_ReturnType result;

    for (int i=0; i<iterations; ++i) {
        result = E2E_P01Check(&config_, &state_, buffer_);
        EXPECT_EQ(result, E2E_E_OK);
    }

    EXPECT_EQ(state_.NoNewOrRepeatedDataCounter, expectedCounterValue);
}

TEST_F(P01Check, NewDataAvailableWithInvalidCRCSetsStatusWRONGCRC) {
    ProtectCurrentBuffer();

    // Simply add one to the current CRC
    const size_t crcByteIndex = config_.CRCOffset / 8;
    buffer_[crcByteIndex] += 1U;

    Std_ReturnType result = E2E_P01Check(&config_, &state_, buffer_);

    EXPECT_EQ(result, E2E_E_OK);
    EXPECT_EQ(state_.Status, E2E_P01STATUS_WRONGCRC);
}

TEST_F(P01Check, FirstReceivedDataSetsStatusINITIAL) {
    ProtectCurrentBuffer();

    Std_ReturnType result = E2E_P01Check(&config_, &state_, buffer_);
    EXPECT_EQ(result, E2E_E_OK);
    EXPECT_EQ(state_.Status, E2E_P01STATUS_INITIAL);
}

TEST_F(P01Check, FirstReceivedDataRemovesWaitForFirstDataFlag) {
    ProtectCurrentBuffer();

    Std_ReturnType result = E2E_P01Check(&config_, &state_, buffer_);
    EXPECT_EQ(result, E2E_E_OK);
    EXPECT_EQ(state_.WaitForFirstData, false);
}

TEST_F(P01Check, FirstReceivedDataSetsMaxDeltaCounter) {
    ProtectCurrentBuffer();

    Std_ReturnType result = E2E_P01Check(&config_, &state_, buffer_);
    EXPECT_EQ(result, E2E_E_OK);
    EXPECT_EQ(state_.MaxDeltaCounter, config_.MaxDeltaCounterInit);
}

TEST_F(P01Check, FirstReceivedDataSetsLastValidCounter) {
    const uint8_t expectedCounterValue = 0xA;
    protect_state_.Counter = expectedCounterValue;

    ProtectCurrentBuffer();

    Std_ReturnType result = E2E_P01Check(&config_, &state_, buffer_);
    EXPECT_EQ(result, E2E_E_OK);
    EXPECT_EQ(state_.LastValidCounter, expectedCounterValue);
}

TEST_F(P01Check, RepeatedDataSetsStatusREPEATED) {
    ProtectCurrentBuffer();

    Std_ReturnType result = E2E_P01Check(&config_, &state_, buffer_);
    EXPECT_EQ(result, E2E_E_OK);
    EXPECT_EQ(state_.Status, E2E_P01STATUS_INITIAL);

    // Same buffer again
    result = E2E_P01Check(&config_, &state_, buffer_);
    EXPECT_EQ(state_.Status, E2E_P01STATUS_REPEATED);
}

TEST_F(P01Check, RepeatedDataCounterIsCappedAtMaxValue) {
    ProtectCurrentBuffer();

    Std_ReturnType result = E2E_P01Check(&config_, &state_, buffer_);
    EXPECT_EQ(result, E2E_E_OK);
    EXPECT_EQ(state_.Status, E2E_P01STATUS_INITIAL);

    EXPECT_EQ(state_.NoNewOrRepeatedDataCounter, 0);

    for (int i=0; i<20; ++i) {
        // Same buffer again
        result = E2E_P01Check(&config_, &state_, buffer_);
        EXPECT_EQ(state_.Status, E2E_P01STATUS_REPEATED);
    }

    EXPECT_EQ(state_.NoNewOrRepeatedDataCounter, 14);
}

TEST_F(P01Check, CounterOverflowIsHandledProperly) {
    protect_state_.Counter = 0xE;
    ProtectCurrentBuffer();
    // Ensure it has overflown, otherwise the test case is moot
    ASSERT_EQ(protect_state_.Counter, 0);

    Std_ReturnType result = E2E_P01Check(&config_, &state_, buffer_);
    EXPECT_EQ(result, E2E_E_OK);
    EXPECT_EQ(state_.Status, E2E_P01STATUS_INITIAL);

    ProtectCurrentBuffer();

    result = E2E_P01Check(&config_, &state_, buffer_);
    EXPECT_EQ(state_.Status, E2E_P01STATUS_OK);
}


/**
 * Given a valid message, if the delta between the current and previous
 * message's counter value exceeds the configured maximum:
 *
 * - the new/repeated data counter is reset,
 * - the SyncCounter is initialized
 *
 * Given the SyncCounter is non-zero
 * - The Max Delta Counter is reinitialized
 * - The last received counter value is remembered as the last valid counter value
 *
 * and the Status is set to E2E_P01STATUS_WRONGSEQUENCE
 *
 * The following tests will establish this behavior
 */

TEST_F(P01Check, CounterDeltaExceededResetRepeatCounter) {
    // Setup the scenario
    ProtectCurrentBuffer();
    Std_ReturnType result = E2E_P01Check(&config_, &state_, buffer_);
    EXPECT_EQ(result, E2E_E_OK);
    EXPECT_EQ(state_.Status, E2E_P01STATUS_INITIAL);
    protect_state_.Counter += config_.MaxDeltaCounterInit + 1;
    ProtectCurrentBuffer();
    // Set the new/repeated data counter to something just to see
    // that the reset is done properly
    state_.NoNewOrRepeatedDataCounter = 5;

    // Run the check
    result = E2E_P01Check(&config_, &state_, buffer_);

    // Assert the results
    EXPECT_EQ(result, E2E_E_OK);
    EXPECT_EQ(state_.Status, E2E_P01STATUS_WRONGSEQUENCE);
    EXPECT_EQ(state_.NoNewOrRepeatedDataCounter, 0);
}

TEST_F(P01Check, CounterDeltaExceededInitializesSyncCounter) {

    // Since the default SyncCounterInit is zero, we change it to
    // establish that it is properly set in the case
    config_.SyncCounterInit = 5;
    ProtectCurrentBuffer();
    Std_ReturnType result = E2E_P01Check(&config_, &state_, buffer_);
    EXPECT_EQ(result, E2E_E_OK);
    EXPECT_EQ(state_.Status, E2E_P01STATUS_INITIAL);
    protect_state_.Counter += config_.MaxDeltaCounterInit + 1;

    ProtectCurrentBuffer();

    result = E2E_P01Check(&config_, &state_, buffer_);
    EXPECT_EQ(state_.Status, E2E_P01STATUS_WRONGSEQUENCE);
    EXPECT_EQ(state_.SyncCounter, config_.SyncCounterInit);
}

TEST_F(P01Check, CounterDeltaExceededResetsLastValidCounterIfSyncCounterIsNonZero) {
    ProtectCurrentBuffer();
    Std_ReturnType result = E2E_P01Check(&config_, &state_, buffer_);
    EXPECT_EQ(result, E2E_E_OK);
    EXPECT_EQ(state_.Status, E2E_P01STATUS_INITIAL);
    protect_state_.Counter += config_.MaxDeltaCounterInit + 1;
    ProtectCurrentBuffer();

    // Value should be unchanged as SyncCounterInit is 0
    result = E2E_P01Check(&config_, &state_, buffer_);
    EXPECT_EQ(state_.Status, E2E_P01STATUS_WRONGSEQUENCE);
    EXPECT_EQ(state_.LastValidCounter, 0);

    ProtectCurrentBuffer();

    config_.SyncCounterInit = 5;
    result = E2E_P01Check(&config_, &state_, buffer_);
    EXPECT_EQ(state_.Status, E2E_P01STATUS_WRONGSEQUENCE);
    EXPECT_EQ(state_.LastValidCounter, protect_state_.Counter - 1);
}

TEST_F(P01Check, CounterDeltaExceededResetsMaxDeltaCounterIfSyncCounterIsNonZero) {
    ProtectCurrentBuffer();
    Std_ReturnType result = E2E_P01Check(&config_, &state_, buffer_);
    EXPECT_EQ(result, E2E_E_OK);
    EXPECT_EQ(state_.Status, E2E_P01STATUS_INITIAL);
    protect_state_.Counter += config_.MaxDeltaCounterInit + 1;
    ProtectCurrentBuffer();

    // Value should be unchanged as SyncCounterInit is 0
    result = E2E_P01Check(&config_, &state_, buffer_);
    EXPECT_EQ(state_.Status, E2E_P01STATUS_WRONGSEQUENCE);
    EXPECT_EQ(state_.MaxDeltaCounter, 2);

    ProtectCurrentBuffer();

    config_.SyncCounterInit = 5;
    result = E2E_P01Check(&config_, &state_, buffer_);
    EXPECT_EQ(state_.Status, E2E_P01STATUS_WRONGSEQUENCE);
    EXPECT_EQ(state_.MaxDeltaCounter, config_.MaxDeltaCounterInit);
}

TEST_F(P01Check, ReceiveCounterExceedingMaxValueReturnsINPUTERR_WRONG) {
    ProtectCurrentBuffer();

    const uint8_t high_nibble = buffer_[1] & 0xF0;
    buffer_[1] = high_nibble | 0x0F;

    Std_ReturnType result = E2E_P01Check(&config_, &state_, buffer_);
    EXPECT_EQ(result, E2E_E_INPUTERR_WRONG);
}

TEST_F(P01Check, RepeatedDataCounterExceedingConfiguredMaxValue) {
    // Case to be tested:
    // if (RepeatedDataCounter > ConfiguredMaxRepetitions) {
    //    Initialize Sync Counter
    //    Reset Repeated Data Counter
    //    Set result to E2E_P01STATUS_SYNC
    // }

    ProtectCurrentBuffer();

    config_.MaxNoNewOrRepeatedData = 1;
    config_.SyncCounterInit = 5;

    Std_ReturnType result = E2E_P01Check(&config_, &state_, buffer_);
    EXPECT_EQ(result, E2E_E_OK);
    EXPECT_EQ(state_.Status, E2E_P01STATUS_INITIAL);

    result = E2E_P01Check(&config_, &state_, buffer_);
    EXPECT_EQ(result, E2E_E_OK);
    EXPECT_EQ(state_.Status, E2E_P01STATUS_REPEATED);

    result = E2E_P01Check(&config_, &state_, buffer_);
    EXPECT_EQ(result, E2E_E_OK);
    EXPECT_EQ(state_.Status, E2E_P01STATUS_REPEATED);

    // The check is performed when new data arrives
    ProtectCurrentBuffer();

    result = E2E_P01Check(&config_, &state_, buffer_);
    EXPECT_EQ(result, E2E_E_OK);
    EXPECT_EQ(state_.SyncCounter, config_.SyncCounterInit);
    EXPECT_EQ(state_.NoNewOrRepeatedDataCounter, 0);
    EXPECT_EQ(state_.Status, E2E_P01STATUS_SYNC);
}


/**
 * When the sync counter is active, the status returned will be
 * E2E_P01STATUS_SYNC for 'SyncCounterInit' number of cycles.
 * (as long as the counter delta does not exceed configured maximum)
 *
 */
TEST_F(P01Check,  MessageReceivedWhenSyncCounterIsActive) {
    // Case to be tested:
    //    if (SyncCounter > 0) {
    //        Decrement SyncCounter
    //        NoNewORRepeatedDataCounter
    //    }

    state_.SyncCounter = 3;

    ProtectCurrentBuffer();
    Std_ReturnType result = E2E_P01Check(&config_, &state_, buffer_);
    EXPECT_EQ(result, E2E_E_OK);
    EXPECT_EQ(state_.SyncCounter, 3);
    EXPECT_EQ(state_.Status, E2E_P01STATUS_INITIAL);

    ProtectCurrentBuffer();
    result = E2E_P01Check(&config_, &state_, buffer_);
    EXPECT_EQ(result, E2E_E_OK);
    EXPECT_EQ(state_.SyncCounter, 2);
    EXPECT_EQ(state_.Status, E2E_P01STATUS_SYNC);

    ProtectCurrentBuffer();
    result = E2E_P01Check(&config_, &state_, buffer_);
    EXPECT_EQ(result, E2E_E_OK);
    EXPECT_EQ(state_.SyncCounter, 1);
    EXPECT_EQ(state_.Status, E2E_P01STATUS_SYNC);

    ProtectCurrentBuffer();
    result = E2E_P01Check(&config_, &state_, buffer_);
    EXPECT_EQ(result, E2E_E_OK);
    EXPECT_EQ(state_.SyncCounter, 0);
    EXPECT_EQ(state_.Status, E2E_P01STATUS_SYNC);

    ProtectCurrentBuffer();
    result = E2E_P01Check(&config_, &state_, buffer_);
    EXPECT_EQ(result, E2E_E_OK);
    EXPECT_EQ(state_.Status, E2E_P01STATUS_OK);
}

/**
 * When receiving a message where the counter delta is more than 1
 * from the previous message, but not exceeding the maximum configured
 * delta, the State shall be E2E_P01STATUS_OKSOMELOST
 */
TEST_F(P01Check,  MessageReceivedWithCounterSkipWithinMaxDelta) {
    ProtectCurrentBuffer();
    Std_ReturnType result = E2E_P01Check(&config_, &state_, buffer_);
    EXPECT_EQ(result, E2E_E_OK);
    EXPECT_EQ(state_.Status, E2E_P01STATUS_INITIAL);

    protect_state_.Counter = 2;
    ProtectCurrentBuffer();
    result = E2E_P01Check(&config_, &state_, buffer_);
    EXPECT_EQ(result, E2E_E_OK);
    EXPECT_EQ(state_.Status, E2E_P01STATUS_OKSOMELOST);
}


TEST_F(P01Check,  MessageReceivedWithDataIDNibbleMismatch) {
    config_.DataIDMode = E2E_P01_DATAID_NIBBLE;
    config_.DataIDNibbleOffset = 12;
    ProtectCurrentBuffer();

    // To mess with this without changing the expected CRC
    // we need to change the DataID in the configuration
    config_.DataID = 0xFF23;

    Std_ReturnType result = E2E_P01Check(&config_, &state_, buffer_);
    EXPECT_EQ(result, E2E_E_OK);
    EXPECT_EQ(state_.Status, E2E_P01STATUS_WRONGCRC);
}
