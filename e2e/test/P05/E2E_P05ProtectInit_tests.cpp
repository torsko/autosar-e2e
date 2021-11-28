/**
 * @file E2E_P05ProtectInit_tests.cpp
 * @author Tomas Wester (torsko@gmail.com)
 * @brief
 * @version 0.1
 * @date 2021-11-23
 *
 *  References
 *
 *  [1] - https://www.autosar.org/fileadmin/user_upload/standards/classic/20-11/AUTOSAR_SWS_E2ELibrary.pdf
 *
 */

extern "C" {
    #include "E2E_P05.h"
}

#include <gtest/gtest.h>


/**
 * Test for [1] SWS_E2E_00448
 *
 * @test In case State is NULL, E2E_P05ProtectInit shall return immediately
 * with E2E_E_INPUTERR_NULL.
 */
TEST(P05ProtectInit,NullInputReturnsINPUTERR_NULL) {
    Std_ReturnType result = E2E_P05ProtectInit(nullptr);
    EXPECT_EQ(result, E2E_E_INPUTERR_NULL);
}


/**
 * Test for [1] SWS_E2E_00448
 *
 * @test Otherwise, it shall intialize the state structure, setting
 * Counter to 0.
 */
TEST(P05ProtectInit, SetsDefaultValues) {
    E2E_P05ProtectStateType state;
    state.Counter = 0xaa;

    Std_ReturnType result = E2E_P05ProtectInit(&state);

    EXPECT_EQ(result, E_OK);
    EXPECT_EQ(state.Counter, 0);
}
