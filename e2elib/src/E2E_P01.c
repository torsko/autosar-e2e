#include "E2E_P01.h"

#include "Crc.h"

static inline uint8_t uint8_min(uint8_t a, uint8_t b) {
    if (a <= b) {
        return a;
    }
    return b;
}

static uint8_t E2E_P01_ReadCounter(const E2E_P01ConfigType* Config,
                                   const uint8_t* Data) {
    uint8_t counter = 0;

    /**
     * PRS_E2E_00663
     * "The offset shall be a multiple of 4"
     */
    if ((Config->CounterOffset % 8) == 0) {
        counter = *(Data+(Config->CounterOffset/8)) & 0x0F;
    } else {
        counter = (*(Data+(Config->CounterOffset/8)) >> 4) & 0x0F;
    }

    return counter;
}

static uint8_t E2E_P01_getDataIdCRC(const E2E_P01ConfigType* Config,
                                    const uint8_t* Data) {
    const uint8_t DataIDMSB = Config->DataID >> 8;
    const uint8_t DataIDLSB = Config->DataID & 0xFF;

    uint8_t CRC;
    switch (Config->DataIDMode) {
        case E2E_P01_DATAID_BOTH:
            CRC = Crc_CalculateCRC8(&DataIDLSB, 1, 0xFF, false);
            CRC = Crc_CalculateCRC8(&DataIDMSB, 1, CRC, false);
            break;
        case E2E_P01_DATAID_LOW:
            CRC = Crc_CalculateCRC8(&DataIDLSB, 1, 0xFF, false);
            break;
        case E2E_P01_DATAID_ALT: {
            uint8_t counter = E2E_P01_ReadCounter(Config, Data);
            if ((counter % 2) == 0) {
                CRC = Crc_CalculateCRC8(&DataIDLSB, 1, 0xFF, false);
            } else {
                CRC = Crc_CalculateCRC8(&DataIDMSB, 1, 0xFF, false);
            }
            break;
        }
        case E2E_P01_DATAID_NIBBLE:
            CRC = Crc_CalculateCRC8(&DataIDLSB, 1, 0xFF, false);
            CRC = Crc_CalculateCRC8(0, 1, CRC, false);
            break;
    }

    return CRC;
}

static uint8_t E2E_P01_CalculateCRCOverDataIdAndData(const E2E_P01ConfigType* Config,
                                                     const uint8_t* Data) {

    uint8_t CRC = E2E_P01_getDataIdCRC(Config, Data);

    // Compute CRC over the area before the CRC (if CRC is not the first byte)
    if (Config->CRCOffset >= 8) {
        CRC = Crc_CalculateCRC8(Data, (Config->CRCOffset/8), CRC, false);
    }

    // Compute the area after CRC, if CRC is not the last byte.
    // Start with the byte after CRC, finish with the last byte of data
    if (((Config->CRCOffset) / 8) < ((Config->DataLength / 8) - 1)) {
        CRC = Crc_CalculateCRC8(&Data[((Config->CRCOffset / 8) + 1)], ((Config->DataLength / 8) - (Config->CRCOffset / 8 - 1)), CRC, false);
    }

    CRC = CRC ^ 0xFF;

    return CRC;
}

static bool E2E_P01_CRCAndDataIDNibble(const E2E_P01ConfigType* Config,
                                       const uint8_t* Data) {
    uint8_t ReceivedDataIDNibble = 0;
    uint8_t DataIDNibble = (Config->DataID >> 8) & 0x0F;

    // Read CRC from Data
    uint8_t ReceivedCRC = *(Data+(Config->CRCOffset/8));

    if (Config->DataIDMode == E2E_P01_DATAID_NIBBLE) {
        // Read low nibble of high byte of Data ID from Data
        if ((Config->DataIDNibbleOffset % 8) == 0) {
            ReceivedDataIDNibble = (*(Data+(Config->DataIDNibbleOffset/8))) & 0x0F;
        } else {
            ReceivedDataIDNibble = (*(Data+(Config->DataIDNibbleOffset/8)) >> 4) & 0x0F;
        }
    }

    uint8_t CalculatedCRC = E2E_P01_CalculateCRCOverDataIdAndData(Config, Data);

    return (ReceivedCRC == CalculatedCRC) && (ReceivedDataIDNibble == DataIDNibble);
}

static void E2E_P01_process_NoNewOrRepeatedDataCounter(E2E_P01CheckStateType* State) {
    if (State->NoNewOrRepeatedDataCounter < 14) {
        State->NoNewOrRepeatedDataCounter++;
    }
}

static E2E_P01CheckStatusType E2E_P01_process_counter(const E2E_P01ConfigType* Config,
                                                      E2E_P01CheckStateType* State,
                                                      uint8_t ReceivedCounter) {
    E2E_P01CheckStatusType result = E2E_P01STATUS_OK;

    uint8_t DeltaCounter = 0;
    if (ReceivedCounter >= State->LastValidCounter) {
        DeltaCounter = ReceivedCounter - State->LastValidCounter;
    } else {
        DeltaCounter = 15 + ReceivedCounter - State->LastValidCounter;
    }

    if (DeltaCounter == 0) {
        // E2E_P01_process_NoNewOrRepeatedDataCounter
        if (State->NoNewOrRepeatedDataCounter < 14) {
            State->NoNewOrRepeatedDataCounter++;
        }
        result = E2E_P01STATUS_REPEATED;
    } else if (DeltaCounter > State->MaxDeltaCounter) {
        // E2E_P01_handle_wrongSequence
        State->NoNewOrRepeatedDataCounter = 0;
        State->SyncCounter = Config->SyncCounterInit;
        if (State->SyncCounter > 0) {
            State->MaxDeltaCounter = Config->MaxDeltaCounterInit;
            State->LastValidCounter = ReceivedCounter;
        }
        result = E2E_P01STATUS_WRONGSEQUENCE;
    } else {
        // E2E_P01_handle_ok_and_okSomeLost
        State->MaxDeltaCounter = Config->MaxDeltaCounterInit;
        State->LastValidCounter = ReceivedCounter;
        State->LostData = DeltaCounter - 1;
        if (State->NoNewOrRepeatedDataCounter > Config->MaxNoNewOrRepeatedData) {
            State->SyncCounter = Config->SyncCounterInit;
            State->NoNewOrRepeatedDataCounter = 0;
            result = E2E_P01STATUS_SYNC;
        } else {
            if (State->SyncCounter > 0) {
                State->SyncCounter--;
                State->NoNewOrRepeatedDataCounter = 0;
                result = E2E_P01STATUS_SYNC;
            } else {
                State->NoNewOrRepeatedDataCounter = 0;
                if (DeltaCounter == 1) {
                    result = E2E_P01STATUS_OK;
                } else {
                    result = E2E_P01STATUS_OKSOMELOST;
                }
            }
        }
    }

    return result;
}

Std_ReturnType E2E_P01Protect(const E2E_P01ConfigType* Config,
                              E2E_P01ProtectStateType* State,
                              uint8_t* DataPtr) {

}

Std_ReturnType E2E_P01ProtectInit(E2E_P01ProtectStateType* State) {
    Std_ReturnType result = E_OK;
    if (NULL == State) {
        result = E2E_E_INPUTERR_NULL;
    } else {
        State->Counter = 0;
    }

    return result;
}

Std_ReturnType E2E_P01Forward(const E2E_P01ConfigType* Config,
                              E2E_PCheckStatusType Checkstatus,
                              E2E_P01ProtectStateType* State,
                              uint8_t* DataPtr) {

}

/**
 * PRS_E2E_00196
 *
 * The function E2E_P01Check shall
 * 1. Check the CRC
 * 2. Check the Data ID nibble, i.e. compare the expected value with the received
 *    value (for E2E_P01_DATAID_NIBBLE configuration only)
 * 3. Check the Counter,
 * 4. determine the check Status,
 *
 * @param Config
 * @param State
 * @param Data
 * @return Std_ReturnType
 */
Std_ReturnType E2E_P01Check(const E2E_P01ConfigType* Config,
                           E2E_P01CheckStateType* State,
                           const uint8_t* Data) {

    Std_ReturnType result = E2E_E_OK;

    // Analyse this, the post-inc would mean that we send in the unmodified
    // MaxDeltaCounter, right?
    State->MaxDeltaCounter = uint8_min(State->MaxDeltaCounter++, 14);

    if (State->NewDataAvailable) {
        uint8_t ReceivedCounter = E2E_P01_ReadCounter(Config, Data);
        if (ReceivedCounter < 15) {
            bool dataValid = E2E_P01_CRCAndDataIDNibble(Config, Data);

            if (dataValid) {
                if (State->WaitForFirstData) {
                    State->WaitForFirstData = false;
                    State->MaxDeltaCounter = Config->MaxDeltaCounterInit;
                    State->LastValidCounter = ReceivedCounter;
                    State->Status = E2E_P01STATUS_INITIAL;
                } else {
                    result = E2E_P01_process_counter(Config, State, ReceivedCounter);
                }
            } else {
                State->Status = E2E_P01STATUS_NONEWDATA;
            }
        } else {
            result = E2E_E_INPUTERR_WRONG;
        }
    } else {
        // E2E_P01_process_NoNewOrRepeatedDataCounter
        if (State->NoNewOrRepeatedDataCounter < 14) {
            State->NoNewOrRepeatedDataCounter++;
        }
        State->Status = E2E_P01STATUS_NONEWDATA;
    }



}

Std_ReturnType E2E_P01CheckInit(E2E_P01CheckStateType* State) {
    Std_ReturnType result = E_OK;
    if (State == NULL) {
        result = E2E_E_INPUTERR_NULL;
    } else {
        State->LastValidCounter = 0;
        State->MaxDeltaCounter = 0;
        State->WaitForFirstData = true;
        State->NewDataAvailable = true;
        State->LostData = 0;
        State->Status = E2E_P01STATUS_NONEWDATA;
        State->NoNewOrRepeatedDataCounter = 0;
        State->SyncCounter = 0;
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
