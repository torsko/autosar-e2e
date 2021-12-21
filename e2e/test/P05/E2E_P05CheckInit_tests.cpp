/**
 * @file E2E_P05CheckInit_tests.cpp
 * @author Tomas Wester (torsko@gmail.com)
 * @brief
 * @version 0.1
 * @date 2021-11-28
 *
 * References
 *
 * [1] https://www.autosar.org/fileadmin/user_upload/standards/classic/20-11/AUTOSAR_SWS_E2ELibrary.pdf
 *
 */

extern "C" {
    #include "E2E_P05.h"
}

#include <gtest/gtest.h>


/**
 * Test 1/2 for [1] SWS_E2E_00451
 *
 * "In case State is NULL, E2E_P05CheckInit shall return immediately
 *  with E2E_E_INPUTERR_NULL."
 *
 * @test E2E_P05CheckInit returns E2E_E_INPUTERR_NULL when provided with
 * a null parameter
 */
TEST(P05CheckInit,NullInputReturnsINPUTERR_NULL) {
    Std_ReturnType result = E2E_P05CheckInit(nullptr);

    EXPECT_EQ(result, E2E_E_INPUTERR_NULL);
}

/**
 * Test 2/2 for [1] SWS_E2E_00451
 *
 * Otherwise, it shall initialize the state structure, setting:
 * 1. Counter to 0xFF
 * 2. Status to E2E_P05STATUS_ERROR.
 *
 * @test E2E_P05CheckInit sets default values
 */
TEST(P05CheckInit, SetsDefaultValues) {
    E2E_P05CheckStateType state;

    Std_ReturnType result = E2E_P05CheckInit(&state);

    EXPECT_EQ(result, E_OK);
    EXPECT_EQ(state.Counter, 0xFF);
    EXPECT_EQ(state.Status, E2E_P05STATUS_ERROR);
}
