/**
 * @file E2E_P05MapStatusToSM_tests.cpp
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
 * Test for [1] SWS_E2E_00453
 *
 * If CheckReturn = E2E_E_OK, then the function
 * E2E_P05MapStatusToSM shall return the values depending on the value of Status:
 * | Status | Return value |
 * |:---------------------------------------------|:--------------------|
 * | E2E_P05STATUS_OK or E2E_P05STATUS_OKSOMELOST | E2E_P_OK            |
 * | E2E_P05STATUS_ERROR                          | E2E_P_ERROR         |
 * | E2E_P05STATUS_REPEATED                       | E2E_P_REPEATED      |
 * | E2E_P05STATUS_NONEWDATA                      | E2E_P_NONEWDATA     |
 * | E2E_P05STATUS_WRONGSEQUENCE                  | E2E_P_WRONGSEQUENCE |
 *
 *
 * @test E2E_P05MapStatusToSM returns corresponding SM states
 * when CheckReturn = E2E_E_OK
 */
TEST(P05MapStatusToSM, MappingWhenCheckReturnIsOK) {
    const Std_ReturnType checkReturn = E2E_E_OK;

    // E2E_P05STATUS_OK --> E2E_P_OK
    E2E_PCheckStatusType result = E2E_P05MapStatusToSM(checkReturn, E2E_P05STATUS_OK);
    EXPECT_EQ(result, E2E_P_OK);

    // E2E_P05STATUS_OKSOMELOST --> E2E_P_OK
    result = E2E_P05MapStatusToSM(checkReturn, E2E_P05STATUS_OKSOMELOST);
    EXPECT_EQ(result, E2E_P_OK);

    // E2E_P05STATUS_ERROR --> E2E_P_ERROR
    result = E2E_P05MapStatusToSM(checkReturn, E2E_P05STATUS_ERROR);
    EXPECT_EQ(result, E2E_P_ERROR);

    // E2E_P05STATUS_REPEATED --> E2E_P_REPEATED
    result = E2E_P05MapStatusToSM(checkReturn, E2E_P05STATUS_REPEATED);
    EXPECT_EQ(result, E2E_P_REPEATED);

    // E2E_P05STATUS_NONEWDATA --> E2E_P_NONEWDATA
    result = E2E_P05MapStatusToSM(checkReturn, E2E_P05STATUS_NONEWDATA);
    EXPECT_EQ(result, E2E_P_NONEWDATA);

    // E2E_P05STATUS_WRONGSEQUENCE --> E2E_P_WRONGSEQUENCE
    result = E2E_P05MapStatusToSM(checkReturn, E2E_P05STATUS_WRONGSEQUENCE);
    EXPECT_EQ(result, E2E_P_WRONGSEQUENCE);
}

/**
 * Test for [1] SWS_E2E_00454
 *
 * If CheckReturn != E2E_E_OK, then the function
 * E2E_P05MapStatusToSM() shall return E2E_P_ERROR (regardless of value of
 * Status).
 *
 * @note This test will iterate over all permutations of statuses combined with
 * non-ok CheckReturn values.
 *
 * @test E2E_P05MapStatusToSM returns E2E_P_ERROR when CheckReturn is not E2E_E_OK
 */
TEST(P05MapStatusToSM, CheckReturnNotOkAlwaysReturnError) {
    // Presumably we need to test for all permutations

    Std_ReturnType non_ok_return_types[] = {
        E2E_E_INPUTERR_NULL,
        E2E_E_INPUTERR_WRONG,
        E2E_E_INTERR,
        E2E_E_WRONGSTATE
    };

    E2E_P05CheckStatusType check_status_types[] = {
        E2E_P05STATUS_OK,
        E2E_P05STATUS_NONEWDATA,
        E2E_P05STATUS_ERROR,
        E2E_P05STATUS_REPEATED,
        E2E_P05STATUS_OKSOMELOST,
        E2E_P05STATUS_WRONGSEQUENCE
    };

    E2E_PCheckStatusType result;
    for (int i=0; i<(sizeof(non_ok_return_types)/sizeof(Std_ReturnType)); ++i) {
        for (int j=0; j<(sizeof(check_status_types)/sizeof(E2E_P05CheckStatusType)); ++j) {
            result = E2E_P05MapStatusToSM(
                non_ok_return_types[i], check_status_types[j]
            );
            EXPECT_EQ(result, E2E_P_ERROR);
        }
    }
}

/**
 * Behavior not defined in reference documentation
 *
 * @test E2E_P05MapStatusToSM returns E2E_P_ERROR given an unknown E2E_P05CheckStatusType is provided as
 * Status parameter.
 */
TEST(P05MapStatusToSM, UnknownStatusReturnsP_ERROR) {
    E2E_PCheckStatusType result = E2E_P05MapStatusToSM(E2E_E_OK, (E2E_P05CheckStatusType)0xff);
    EXPECT_EQ(result, E2E_P_ERROR);
}
