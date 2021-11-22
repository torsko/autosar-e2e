#include "E2E_SM.h"


static uint8_t E2E_SMGetStatusCount(E2E_PCheckStatusType ProfileStatus,
                                    E2E_SMCheckStateType* StatePtr,
                                    uint8_t CurrentWindowSize) {
    uint8_t count = 0;
    for (uint8_t i=0; i<CurrentWindowSize; ++i) {
        if (StatePtr->ProfileStatusWindow[i] == ProfileStatus) {
            count++;
        }
    }

    return count;
}

/**
 * Convenience function added to get current window size depending on
 * state machine state.
 *
 * @param StatePtr
 * @param ConfigPtr
 * @return uint8_t
 */
static uint8_t E2E_SMGetWindowSize(E2E_SMStateType SMState,
                                   const E2E_SMConfigType* ConfigPtr) {

    uint8_t result = 0;

    switch (SMState) {
        case E2E_SM_VALID:
            result = ConfigPtr->WindowSizeValid;
            break;
        case E2E_SM_INVALID:
            result = ConfigPtr->WindowSizeInvalid;
            break;
        case E2E_SM_INIT:
            result = ConfigPtr->WindowSizeInit;
            break;
        // Considering NODATA an initialization state.
        // However, this could also be considered a separate case with a constant
        // configuration independent value.
        // Further analysis is required
        case E2E_SM_NODATA:
            result = ConfigPtr->WindowSizeInit;
        default:
            // TBD
            // A window size of 0 would be returned,
            // analysis is required to see what this implies.
            break;
    }

    return result;
}

/**
 * E2E_SMAddStatus is just a logical step in the algorithm, it may (but it does not have
 * to be) implemented as a separate function. It is not a module API function. The value
 * State->OKCount represents the number of received E2E_P_OK status. The value
 * State->ErrorCounter represents the number of E2E_P_ERROR status. The remaining
 * status values represent counter errors, which don’t contribute to State->OKCount or
 * State->ErrorCount.
 *
 * Implemented according to PRS_E2E_00466 as a static function
 *
 * @note The step is described in PRS_E2E_00466 without the configuration
 * as a parameter, but it is used to determine CurrentWindowSize, hence
 * it is included as a parameter here.
 *
 * @param ProfileStatus
 * @param StatePtr
 */
static void E2E_SMAddStatus(E2E_PCheckStatusType ProfileStatus,
                            const E2E_SMConfigType* ConfigPtr,
                            E2E_SMCheckStateType* StatePtr) {
    StatePtr->ProfileStatusWindow[StatePtr->WindowTopIndex] = ProfileStatus;
    uint8_t CurrentWindowSize = E2E_SMGetWindowSize(StatePtr->SMState, ConfigPtr);

    StatePtr->OkCount = E2E_SMGetStatusCount(E2E_P_OK, StatePtr, CurrentWindowSize);
    StatePtr->ErrorCount = E2E_SMGetStatusCount(E2E_P_ERROR, StatePtr, CurrentWindowSize);

    if (StatePtr->WindowTopIndex == (CurrentWindowSize - 1U)) {
        StatePtr->WindowTopIndex = 0;
    } else {
        StatePtr->WindowTopIndex++;
    }
}

/**
 * PRS_E2E_00467
 *
 * @note Using WindowSizeValid from configuration to reset the
 * ProfileStatusWindow contents. This is based on the constraints
 * constr_6301 and constr_6302 which implies that WindowSizeValid
 * shall always be the biggest (or as big as) the other WindowSize
 * definitions
 *
 * @param StatePtr
 * @param ConfigPtr
 */
static void E2E_SMClearStatus(E2E_SMCheckStateType* StatePtr,
                              const E2E_SMConfigType* ConfigPtr) {

    for (uint8_t i=0; i<ConfigPtr->WindowSizeValid; ++i) {
        StatePtr->ProfileStatusWindow[i] = E2E_P_NOTAVAILABLE;
    }
    StatePtr->OkCount = 0;
    StatePtr->ErrorCount = 0;
    StatePtr->WindowTopIndex = 0;
}

/**
 * PRS_E2E_00607
 *
 * @param StatePtr
 * @param ConfigPtr
 * @param NextState
 */
// static void E2E_SMClearRemainingStatus(E2E_SMCheckStateType* StatePtr,
//                                        const E2E_SMConfigType* ConfigPtr,
//                                        E2E_SMStateType NextState) {
//     uint8_t CurrentWindowSize = E2E_SMGetWindowSize(StatePtr->SMState, ConfigPtr);
//     uint8_t NextWindowSize = E2E_SMGetWindowSize(StatePtr->SMState, ConfigPtr);

    // TBD
    // The state diagram is unclear, I think the intent is as below
    // but I need to consider it with a significantly higher dose of
    // caffeine in my veins.

    // Also, the handling of WindowTopIndex while transitioning between
    // IDLE/VALID/INVALID with different window sizes needs to be clarified.
    // Based on the state diagram we need to consider wrap-around instead
    // of just keeping the "CurrentWindowSize" first elements of the array.


    // if (CurrentWindowSize < NextWindowSize) {
    //     set all elements in ProfileStatusWindow to E2E_P_NOTAVAILABLE
    //     except the latest n elements
    // }
// }

static void E2E_SMCheck_NODATA(E2E_PCheckStatusType ProfileStatus,
                               const E2E_SMConfigType* ConfigPtr,
                               E2E_SMCheckStateType* StatePtr) {
    if (StatePtr->WindowTopIndex == 0) {
        // This indicates a WindowTopIndex overflow
        // i.e. only Error or NoNewData ProfileStatuses have been reported
        // up until this point

        if (ConfigPtr->ClearToInvalid) {
            E2E_SMClearStatus(StatePtr, ConfigPtr);
        } else {
            // E2E_SMClearRemainingStatus(StatePtr, ConfigPtr, E2E_SM_INVALID);
        }
        // Transition to INVALID
        StatePtr->SMState = E2E_SM_INVALID;
    } else if ((ProfileStatus != E2E_P_ERROR) &&
               (ProfileStatus != E2E_P_NONEWDATA)) {
        // Something was received!
        // Clear window
        E2E_SMClearStatus(StatePtr, ConfigPtr);
        // Transition to INIT
        StatePtr->SMState = E2E_SM_INIT;
    } else {
        //  Simply remain in this state
    }
}

// cppcheck-suppress unusedFunction
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

// cppcheck-suppress unusedFunction
Std_ReturnType E2E_SMCheck(E2E_PCheckStatusType ProfileStatus,
                           const E2E_SMConfigType* ConfigPtr,
                           E2E_SMCheckStateType* StatePtr) {
    Std_ReturnType result = E2E_E_OK;

    if ((ConfigPtr == NULL) || (StatePtr == NULL)) {
        result = E2E_E_INPUTERR_NULL;
    } else if (StatePtr->SMState == E2E_SM_DEINIT) {
        result = E2E_E_WRONGSTATE;
    }
    else {
        E2E_SMAddStatus(ProfileStatus, ConfigPtr, StatePtr);

        switch (StatePtr->SMState) {
            case E2E_SM_NODATA:
                E2E_SMCheck_NODATA(ProfileStatus, ConfigPtr, StatePtr);
            break;
            default: // remove this later
            break;
        }
    }

    return result;
}
