extern "C" {
    #include "E2E_P01.h"
}

#include <gtest/gtest.h>

TEST(P01MapStatusToSM, ValidMapping) {
    const Std_ReturnType checkReturn = E2E_E_OK;
    const bool profileBehavior = false;

    // E2E_P01STATUS_OK --> E2E_P_OK
    E2E_PCheckStatusType result = E2E_P01MapStatusToSM(
        checkReturn, E2E_P01STATUS_OK, profileBehavior
    );
    EXPECT_EQ(result, E2E_P_OK);

    // E2E_P01STATUS_OKSOMELOST --> E2E_P_OK
    result = E2E_P01MapStatusToSM(
        checkReturn, E2E_P01STATUS_OKSOMELOST, profileBehavior
    );
    EXPECT_EQ(result, E2E_P_OK);

    // E2E_P01STATUS_INITIAL --> E2E_P_OK
    result = E2E_P01MapStatusToSM(
        checkReturn, E2E_P01STATUS_INITIAL, profileBehavior
    );
    EXPECT_EQ(result, E2E_P_OK);

    // E2E_P01STATUS_WRONGCRC --> E2E_P_ERROR
    result = E2E_P01MapStatusToSM(
        checkReturn, E2E_P01STATUS_WRONGCRC, profileBehavior
    );
    EXPECT_EQ(result, E2E_P_ERROR);

    // E2E_P01STATUS_REPEATED --> E2E_P_REPEATED
    result = E2E_P01MapStatusToSM(
        checkReturn, E2E_P01STATUS_REPEATED, profileBehavior
    );
    EXPECT_EQ(result, E2E_P_REPEATED);

    // E2E_P01STATUS_NONEWDATA --> E2E_P_NONEWDATA
    result = E2E_P01MapStatusToSM(
        checkReturn, E2E_P01STATUS_NONEWDATA, profileBehavior
    );
    EXPECT_EQ(result, E2E_P_NONEWDATA);

    // E2E_P01STATUS_WRONGSEQUENCE --> E2E_P_WRONGSEQUENCE
    result = E2E_P01MapStatusToSM(
        checkReturn, E2E_P01STATUS_WRONGSEQUENCE, profileBehavior
    );
    EXPECT_EQ(result, E2E_P_WRONGSEQUENCE);

    // E2E_P01STATUS_SYNC --> E2E_P_WRONGSEQUENCE
    result = E2E_P01MapStatusToSM(
        checkReturn, E2E_P01STATUS_SYNC, profileBehavior
    );
    EXPECT_EQ(result, E2E_P_WRONGSEQUENCE);
}

TEST(P01MapStatusToSM, ValidMappingPreR42Behavior) {
    const Std_ReturnType checkReturn = E2E_E_OK;
    const bool profileBehavior = true;

    // E2E_P01STATUS_OK --> E2E_P_OK
    E2E_PCheckStatusType result = E2E_P01MapStatusToSM(
        checkReturn, E2E_P01STATUS_OK, profileBehavior
    );
    EXPECT_EQ(result, E2E_P_OK);

    // E2E_P01STATUS_OKSOMELOST --> E2E_P_OK
    result = E2E_P01MapStatusToSM(
        checkReturn, E2E_P01STATUS_OKSOMELOST, profileBehavior
    );
    EXPECT_EQ(result, E2E_P_OK);

    // E2E_P01STATUS_SYNC --> E2E_P_OK
    result = E2E_P01MapStatusToSM(
        checkReturn, E2E_P01STATUS_SYNC, profileBehavior
    );
    EXPECT_EQ(result, E2E_P_OK);

    // E2E_P01STATUS_WRONGCRC --> E2E_P_ERROR
    result = E2E_P01MapStatusToSM(
        checkReturn, E2E_P01STATUS_WRONGCRC, profileBehavior
    );
    EXPECT_EQ(result, E2E_P_ERROR);

    // E2E_P01STATUS_REPEATED --> E2E_P_REPEATED
    result = E2E_P01MapStatusToSM(
        checkReturn, E2E_P01STATUS_REPEATED, profileBehavior
    );
    EXPECT_EQ(result, E2E_P_REPEATED);

    // E2E_P01STATUS_NONEWDATA --> E2E_P_NONEWDATA
    result = E2E_P01MapStatusToSM(
        checkReturn, E2E_P01STATUS_NONEWDATA, profileBehavior
    );
    EXPECT_EQ(result, E2E_P_NONEWDATA);

    // E2E_P01STATUS_WRONGSEQUENCE --> E2E_P_WRONGSEQUENCE
    result = E2E_P01MapStatusToSM(
        checkReturn, E2E_P01STATUS_WRONGSEQUENCE, profileBehavior
    );
    EXPECT_EQ(result, E2E_P_WRONGSEQUENCE);

    // E2E_P01STATUS_INITIAL --> E2E_P_WRONGSEQUENCE
    result = E2E_P01MapStatusToSM(
        checkReturn, E2E_P01STATUS_INITIAL, profileBehavior
    );
    EXPECT_EQ(result, E2E_P_WRONGSEQUENCE);
}

TEST(P01MapStatusToSM, CheckReturnNotOkAlwaysReturnError) {
    // Presumably we need to test for all permutations

    Std_ReturnType non_ok_return_types[] = {
        E2E_E_INPUTERR_NULL,
        E2E_E_INPUTERR_WRONG,
        E2E_E_INTERR,
        E2E_E_WRONGSTATE
    };

    E2E_P01CheckStatusType check_status_types[] = {
        E2E_P01STATUS_OK,
        E2E_P01STATUS_NONEWDATA,
        E2E_P01STATUS_WRONGCRC,
        E2E_P01STATUS_SYNC,
        E2E_P01STATUS_INITIAL,
        E2E_P01STATUS_REPEATED,
        E2E_P01STATUS_OKSOMELOST,
        E2E_P01STATUS_WRONGSEQUENCE
    };

    E2E_PCheckStatusType result;
    for (int i=0; i<(sizeof(non_ok_return_types)/sizeof(Std_ReturnType)); ++i) {
        for (int j=0; j<(sizeof(check_status_types)/sizeof(E2E_P01CheckStatusType)); ++j) {

            // Check for both current and pre-R4.2 behavior
            result = E2E_P01MapStatusToSM(
                non_ok_return_types[i], check_status_types[j], true
            );
            EXPECT_EQ(result, E2E_P_ERROR);

            result = E2E_P01MapStatusToSM(
                non_ok_return_types[i], check_status_types[j], false
            );
            EXPECT_EQ(result, E2E_P_ERROR);
        }
    }
}
