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
    state.ProfileStatusWindow = profile_status_window.data();

    Std_ReturnType result = E2E_SMCheckInit(&state, &config);
    EXPECT_EQ(result, E2E_E_OK);
    for (size_t i = 0; i < window_size; ++i) {
        EXPECT_EQ(profile_status_window[i], E2E_P_NOTAVAILABLE);
    }
    EXPECT_EQ(profile_status_window[5], 0);
}

/**
 *  SWS_E2E_00371
 */
TEST(SMCheck, NullConfigParameterReturnsError) {
    E2E_SMCheckStateType state;

    Std_ReturnType result = E2E_SMCheck(E2E_P_OK, NULL, &state);
    EXPECT_EQ(result, E2E_E_INPUTERR_NULL);
}

/**
 *  SWS_E2E_00371
 */
TEST(SMCheck, NullStateParameterReturnsError) {
    E2E_SMConfigType config;

    Std_ReturnType result = E2E_SMCheck(E2E_P_OK, &config, NULL);
    EXPECT_EQ(result, E2E_E_INPUTERR_NULL);
}
