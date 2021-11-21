extern "C" {
    #include "E2E_P01.h"
}

#include <gtest/gtest.h>

TEST(P01CheckInit, SetsDefaultValues) {
    E2E_P01CheckStateType state;

    Std_ReturnType result = E2E_P01CheckInit(&state);

    EXPECT_EQ(result, E_OK);
    EXPECT_EQ(state.LastValidCounter, 0);
    EXPECT_EQ(state.MaxDeltaCounter, 0);
    EXPECT_EQ(state.WaitForFirstData, true);
    EXPECT_EQ(state.NewDataAvailable, true);
    EXPECT_EQ(state.LostData, 0);
    EXPECT_EQ(state.Status, E2E_P01STATUS_NONEWDATA);
    EXPECT_EQ(state.NoNewOrRepeatedDataCounter, 0);
    EXPECT_EQ(state.SyncCounter, 0);
}

TEST(P01CheckInit,NullInputReturnsINPUTERR_NULL) {
    Std_ReturnType result = E2E_P01CheckInit(nullptr);

    EXPECT_EQ(result, E2E_E_INPUTERR_NULL);
}
