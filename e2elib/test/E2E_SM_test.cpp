extern "C" {
    #include "E2E_SM.h"
}

#include <gtest/gtest.h>

/**
 * SWS_E2E_00370
 */
TEST(SMCheckInit, NullConfigParameterReturnsError) {
    E2E_SMCheckStateType state;

    Std_ReturnType result = E2E_SMCheckInit(&state, NULL);
    EXPECT_EQ(result, E2E_E_INPUTERR_NULL);
}

/**
 * SWS_E2E_00370
 */
TEST(SMCheckInit, NullStateParameterReturnsError) {
    E2E_SMConfigType config;

    Std_ReturnType result = E2E_SMCheckInit(NULL, &config);
    EXPECT_EQ(result, E2E_E_INPUTERR_NULL);
}

/**
 * SWS_E2E_00370
 */
TEST(SMCheckInit, ValidParametersInitializesState) {
    const size_t window_size = 5;
    E2E_SMConfigType config;
    config.WindowSizeValid = window_size;

    // Adding an extra element to the array
    // The initialization should only touch the first "window_size" elements.
    std::array<uint8_t, (window_size + 1)> profile_status_window{0};

    E2E_SMCheckStateType state;
    // Setting non-zero starting values to make sure that they are
    // set properly by E2E_SMCheckInit
    state.ErrorCount = 0xcc;
    state.OkCount = 0xcc;
    state.ErrorCount = 0xcc;
    state.SMState = E2E_SM_DEINIT;

    state.ProfileStatusWindow = profile_status_window.data();

    Std_ReturnType result = E2E_SMCheckInit(&state, &config);

    // Verify thta the profile_status_window array is initialized
    EXPECT_EQ(result, E2E_E_OK);
    for (size_t i = 0; i < window_size; ++i) {
        EXPECT_EQ(profile_status_window[i], E2E_P_NOTAVAILABLE);
    }
    // Verify that the extra status field remains untouched.
    EXPECT_EQ(profile_status_window[5], 0);

    // Verify that the ErrorCount is initialized to 0
    EXPECT_EQ(state.ErrorCount, 0);

    // Verify that the OkCount is initialized to 0
    EXPECT_EQ(state.OkCount, 0);

    // Verify that the WindowTopIndex is initialized to 0
    EXPECT_EQ(state.WindowTopIndex, 0);

    // Verify that the SMState is initialized to E2E_SM_NODATA
    EXPECT_EQ(state.SMState, E2E_SM_NODATA);
}


class SMCheck : public ::testing::Test {
protected:
    static const uint8_t kWindowSizeInit = 10;
    static const uint8_t kWindowSizeValid = 20;
    static const uint8_t kWindowSizeInvalid = 5;

    void SetUp() override {
        config_.WindowSizeInit = kWindowSizeInit;
        config_.WindowSizeValid = kWindowSizeValid;
        config_.WindowSizeInvalid = kWindowSizeInvalid;

        state_.ProfileStatusWindow = profile_status_window_.data();
        profile_status_window_.fill(0xFF);
    }

    E2E_SMCheckStateType state_;
    E2E_SMConfigType config_;
    std::array<uint8_t, kWindowSizeValid> profile_status_window_;
};

/**
 *  SWS_E2E_00371
 */
TEST_F(SMCheck, NullConfigParameterReturnsError) {
    Std_ReturnType result = E2E_SMCheck(E2E_P_OK, NULL, &state_);
    EXPECT_EQ(result, E2E_E_INPUTERR_NULL);
}

/**
 *  SWS_E2E_00371
 */
TEST_F(SMCheck, NullStateParameterReturnsError) {
    Std_ReturnType result = E2E_SMCheck(E2E_P_OK, &config_, NULL);
    EXPECT_EQ(result, E2E_E_INPUTERR_NULL);
}


/**
 * The following tests are described with the state machine in Figure 6.244: E2E state machine check
 * from https://www.autosar.org/fileadmin/user_upload/standards/foundation/20-11/AUTOSAR_PRS_E2EProtocol.pdf
 * in mind.
 *
 * Each test name lists the current state, method call, and expected state or return value.
 * The names exclude E2E_SM and E2E_E prefixes.
 *
 *
 */

/**
 * Having the SMState E2E_SM_DEINIT in the E2E_SMCheckStateType struct designates an uninitialized
 * state machine (pre E2E_SMCheckInit).
 * Calling E2E_SMCheck in this state should return E2E_E_WRONGSTATE
 */
TEST_F(SMCheck, DEINITCheckReturnsWRONGSTATE) {
    state_.SMState = E2E_SM_DEINIT;

    Std_ReturnType result = E2E_SMCheck(E2E_P_OK, &config_, &state_);

    EXPECT_EQ(result, E2E_E_WRONGSTATE);
}


/**
 * When in NODATA, calling Check with ProfileStatus E2E_P_NONEWDATA
 * should simply update the profile status window and remain in NODATA
 */
TEST_F(SMCheck, NODATACheckWithNONEWDATARemainsInNODATA) {
    E2E_SMCheckInit(&state_, &config_);

    Std_ReturnType result = E2E_SMCheck(E2E_P_NONEWDATA, &config_, &state_);

    EXPECT_EQ(result, E2E_E_OK);
    EXPECT_EQ(state_.SMState, E2E_SM_NODATA);
}

/**
 * When in NODATA, calling Check with ProfileStatus E2E_P_ERROR
 * should simply update the profile status window and remain in NODATA
 */
TEST_F(SMCheck, NODATACheckWithERRORRemainsInNODATA) {
    E2E_SMCheckInit(&state_, &config_);

    Std_ReturnType result = E2E_SMCheck(E2E_P_ERROR, &config_, &state_);

    EXPECT_EQ(result, E2E_E_OK);
    EXPECT_EQ(state_.SMState, E2E_SM_NODATA);
}

/**
 * When in NODATA, calling Check with ProfileStatus E2E_P_OK
 * should transition to INIT
 */
TEST_F(SMCheck, NODATACheckWithOKRemainsInNODATA) {
    E2E_SMCheckInit(&state_, &config_);

    Std_ReturnType result = E2E_SMCheck(E2E_P_OK, &config_, &state_);

    EXPECT_EQ(result, E2E_E_OK);
    EXPECT_EQ(state_.SMState, E2E_SM_INIT);
}

/**
 * When in NODATA, less than WindowSizeInit consecutive calls to
 * Check with ProfileStatus E2E_P_NONEWDATA fills the profile
 * status window up to WindowSizeInit-1 with E2E_P_NONEWDATA
 * After that, the state machine transitions to INVALID
 */
TEST_F(SMCheck, NODATAConsecutiveCheckWithNONEWDATATransitionToINVALID) {
    E2E_SMCheckInit(&state_, &config_);

    // The NODATA state uses the init window size
    // Fill up all but one with E2E_P_NONEWDATA
    for (uint8_t i=0; i<(kWindowSizeInit - 1U); ++i) {
        Std_ReturnType result = E2E_SMCheck(E2E_P_NONEWDATA, &config_, &state_);
        EXPECT_EQ(result, E2E_E_OK);
        EXPECT_EQ(state_.SMState, E2E_SM_NODATA);
    }

    // Next call should transition to INVALID state
    Std_ReturnType result = E2E_SMCheck(E2E_P_NONEWDATA, &config_, &state_);
    EXPECT_EQ(result, E2E_E_OK);
    EXPECT_EQ(state_.SMState, E2E_SM_INVALID);
}

/**
 * When in NODATA, less than WindowSizeInit consecutive calls to
 * Check with ProfileStatus E2E_P_ERROR fills the profile
 * status window up to WindowSizeInit-1 with E2E_P_ERROR
 * After that, the state machine transitions to INVALID
 */
TEST_F(SMCheck, NODATAConsecutiveCheckWithERRORTransitionToINVALID) {
    E2E_SMCheckInit(&state_, &config_);

    // The NODATA state uses the init window size
    // Fill up all but one with E2E_P_NONEWDATA
    for (uint8_t i=0; i<(kWindowSizeInit - 1U); ++i) {
        Std_ReturnType result = E2E_SMCheck(E2E_P_ERROR, &config_, &state_);
        EXPECT_EQ(result, E2E_E_OK);
        EXPECT_EQ(state_.SMState, E2E_SM_NODATA);
    }

    // Next call should transition to INVALID state
    Std_ReturnType result = E2E_SMCheck(E2E_P_ERROR, &config_, &state_);
    EXPECT_EQ(result, E2E_E_OK);
    EXPECT_EQ(state_.SMState, E2E_SM_INVALID);
}
