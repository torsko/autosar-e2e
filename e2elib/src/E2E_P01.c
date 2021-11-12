#include "E2E.h"

Std_ReturnType E2E_P01Protect(const E2E_P01ConfigType* ConfigPtr,
                              E2E_P01ProtectStateType* StatePtr,
                              uint8_t* DataPtr) {

}

Std_ReturnType E2E_P01ProtectInit(E2E_P01ProtectStateType* StatePtr) {
    Std_ReturnType result = E_OK;
    if (NULL == StatePtr) {
        result = E2E_E_INPUTERR_NULL;
    } else {
        StatePtr->Counter = 0;
    }

    return result;
}

Std_ReturnType E2E_P01Forward(const E2E_P01ConfigType* ConfigPtr,
                              E2E_PCheckStatusType Checkstatus,
                              E2E_P01ProtectStateType* StatePtr,
                              uint8_t* DataPtr) {

}

Std_ReturnType E2E_P01Check(const E2E_P01ConfigType* Config,
                           E2E_P01CheckStateType* State,
                           const uint8_t* Data) {

}

Std_ReturnType E2E_P01CheckInit(E2E_P01CheckStateType* StatePtr) {
    Std_ReturnType result = E_OK;
    if (StatePtr == NULL) {
        result = E2E_E_INPUTERR_NULL;
    } else {
        StatePtr->LastValidCounter = 0;
        StatePtr->MaxDeltaCounter = 0;
        StatePtr->WaitForFirstData = true;
        StatePtr->NewDataAvailable = true;
        StatePtr->LostData = 0;
        StatePtr->Status = E2E_P01STATUS_NONEWDATA;
        StatePtr->NoNewOrRepeatedDataCounter = 0;
        StatePtr->SyncCounter = 0;
    }


    return result;
}

E2E_PCheckStatusType E2E_P01MapStatusToSM (Std_ReturnType CheckReturn,
                                             E2E_P01CheckStatusType Status,
                                             bool profileBehavior) {

    E2E_PCheckStatusType result = E2E_P_OK;

    if (E2E_E_OK != CheckReturn) {
        result = E2E_P_ERROR;
    } else {
        if (profileBehavior) {
            switch (Status) {
                case E2E_P01STATUS_OK:          // Intentional fall-through
                case E2E_P01STATUS_OKSOMELOST:  // Intentional fall-through
                case E2E_P01STATUS_SYNC:
                    result = E2E_P_OK;
                    break;
                case E2E_P01STATUS_WRONGCRC:
                    result = E2E_P_ERROR;
                    break;
                case E2E_P01STATUS_REPEATED:
                    result = E2E_P_REPEATED;
                    break;
                case E2E_P01STATUS_NONEWDATA:
                    result = E2E_P_NONEWDATA;
                    break;
                case E2E_P01STATUS_WRONGSEQUENCE:   // Intentional fall-through
                case E2E_P01STATUS_INITIAL:
                    result = E2E_P_WRONGSEQUENCE;
                    break;

                default:
                    // Undefined, is there such an error code or should the generic error code be used?
                    break;
            }
        } else {
            switch (Status) {
                case E2E_P01STATUS_OK:          // Intentional fall-through
                case E2E_P01STATUS_OKSOMELOST:  // Intentional fall-through
                case E2E_P01STATUS_INITIAL:
                    result = E2E_P_OK;
                    break;
                case E2E_P01STATUS_WRONGCRC:
                    result = E2E_P_ERROR;
                    break;
                case E2E_P01STATUS_REPEATED:
                    result = E2E_P_REPEATED;
                    break;
                case E2E_P01STATUS_NONEWDATA:
                    result = E2E_P_NONEWDATA;
                    break;
                case E2E_P01STATUS_WRONGSEQUENCE:   // Intentional fall-through
                case E2E_P01STATUS_SYNC:
                    result = E2E_P_WRONGSEQUENCE;
                    break;

                default:
                    // Undefined, is there such an error code or should the generic error code be used?
                    break;
            }
        }
    }



    return result;
}
