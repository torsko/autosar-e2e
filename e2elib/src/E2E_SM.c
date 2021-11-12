#include "E2E_SM.h"

Std_ReturnType E2E_SMCheckInit(E2E_SMCheckStateType* StatePtr,
                               const E2E_SMConfigType* ConfigPtr) {
    Std_ReturnType result = E2E_E_OK;

    if ((ConfigPtr == NULL) || (StatePtr == NULL)) {
        result = E2E_E_INPUTERR_NULL;
    } else {
        for (int i=0; i<ConfigPtr->WindowSizeValid; ++i) {
            StatePtr->ProfileStatusWindow[i] = E2E_P_NOTAVAILABLE;
        }
        StatePtr->WindowTopIndex = 0;
        StatePtr->OkCount = 0;
        StatePtr->ErrorCount = 0;
        StatePtr->SMState = E2E_SM_NODATA;
    }

    return result;
}

Std_ReturnType E2E_SMCheck(E2E_PCheckStatusType ProfileStatus,
                           const E2E_SMConfigType* ConfigPtr,
                           E2E_SMCheckStateType* StatePtr) {
    Std_ReturnType result = E2E_E_OK;

    if ((ConfigPtr == NULL) || (StatePtr == NULL)) {
        result = E2E_E_INPUTERR_NULL;
    } else {

    }

    return result;
}
