extern "C" {
    #include "E2E_P01.h"
}

#include <gtest/gtest.h>

TEST(P01ProtectInit, SetsDefaultValues) {
    E2E_P01ProtectStateType state;

    Std_ReturnType result = E2E_P01ProtectInit(&state);

    EXPECT_EQ(result, E_OK);
    EXPECT_EQ(state.Counter, 0);
}

TEST(P01ProtectInit,NullInputReturnsINPUTERR_NULL) {
    Std_ReturnType result = E2E_P01ProtectInit(nullptr);

    EXPECT_EQ(result, E2E_E_INPUTERR_NULL);
}
