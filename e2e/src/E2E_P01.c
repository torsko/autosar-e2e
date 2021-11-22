/**
 * @file E2E_P01.c
 * @author Tomas Wester (torsko@gmail.com)
 * @brief
 * @version 0.1
 * @date 2021-11-21
 *
 * @copyright Copyright (c) 2021
 *
 * @note static functions that are prefixed with "E2E_P01" are named after a corresponding
 *       state machine in the AUTOSAR reference documentation
 *
 * references:
 * - https://www.autosar.org/fileadmin/user_upload/standards/foundation/20-11/AUTOSAR_PRS_E2EProtocol.pdf
 *
 */
#include "E2E_P01.h"

#include "Crc.h"

static inline uint8_t uint8_min(uint8_t a, uint8_t b) {
    if (a <= b) {
        return a;
    }
    return b;
}

static bool configValid(const E2E_P01ConfigType* Config) {
    bool result = true;

    if (((Config->CounterOffset % 4) != 0) ||
        ((Config->CRCOffset % 8) != 0)) {
        result = false;
    }

    switch (Config->DataIDMode) {
        case E2E_P01_DATAID_BOTH:
        case E2E_P01_DATAID_LOW:
        case E2E_P01_DATAID_ALT:
            if (Config->DataIDNibbleOffset != 0) {
                result = false;
            }
        break;
        case E2E_P01_DATAID_NIBBLE:
            if ((Config->DataIDNibbleOffset % 4) != 0) {
                result = false;
            }
            break;
        default:
            result = false;
            break;
    }

    // Check DataLength, it must be a multiple of 8
    // and big enough to hold at least the CRC byte + the
    // counter nibble, (i.e. 2 bytes)
    if (((Config->DataLength % 8) != 0) ||
        (Config->DataLength < 12U)) {
        result = false;
    }



    return result;
}

static uint8_t readCounter(const E2E_P01ConfigType* Config,
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

static void writeCounter(const E2E_P01ConfigType* Config, E2E_P01ProtectStateType* State, uint8_t* Data) {
    const size_t counterByteIndex = Config->CounterOffset / 8U;
    if ((Config->CounterOffset % 8) == 0) {
        const uint8_t oldDataToKeep = Data[counterByteIndex] & 0xF0;
        Data[counterByteIndex] = oldDataToKeep | (State->Counter & 0x0F);
    } else {
        const uint8_t oldDataToKeep = Data[counterByteIndex] & 0x0F;
        Data[counterByteIndex] = oldDataToKeep | ((State->Counter << 4) & 0xF0);
    }
}

static void writeDataIdNibble(const E2E_P01ConfigType* Config, uint8_t* Data) {
    if (Config->DataIDMode == E2E_P01_DATAID_NIBBLE) {
            const size_t dataIdNibbleByteIndex = Config->DataIDNibbleOffset / 8U;
            if ((Config->DataIDNibbleOffset % 8) == 0) {
                const uint8_t oldDataToKeep = Data[dataIdNibbleByteIndex] & 0xF0;
                Data[dataIdNibbleByteIndex] = oldDataToKeep | ((Config->DataID >> 8) & 0x0F);
            } else {
                const uint8_t oldDataToKeep = Data[dataIdNibbleByteIndex] & 0x0F;
                Data[dataIdNibbleByteIndex] = oldDataToKeep | ((Config->DataID >> 4) & 0xF0);
            }
        }
}

static void incrementCounter(E2E_P01ProtectStateType* State) {
    State->Counter++;
    if (State->Counter >= 15) {
        State->Counter = 0;
    }
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
            uint8_t counter = readCounter(Config, Data);
            if ((counter % 2) == 0) {
                CRC = Crc_CalculateCRC8(&DataIDLSB, 1, 0xFF, false);
            } else {
                CRC = Crc_CalculateCRC8(&DataIDMSB, 1, 0xFF, false);
            }
            break;
        }
        case E2E_P01_DATAID_NIBBLE: {
            const uint8_t zero = 0;
            CRC = Crc_CalculateCRC8(&DataIDLSB, 1, 0xFF, false);
            CRC = Crc_CalculateCRC8(&zero, 1, CRC, false);
            break;
        }

        // GCOV_EXCL_START
        default:
            // This case is technically unreachable as the configuration
            // is verified before it is used here.
            break;
        // GCOV_EXCL_STOP
    }

    return CRC;
}

static uint8_t E2E_P01_CalculateCRCOverDataIdAndData(const E2E_P01ConfigType* Config,
                                                     const uint8_t* Data) {

    const size_t crcByteIndex = Config->CRCOffset / 8U;
    const size_t dataLength = Config->DataLength / 8U;

    uint8_t CRC = E2E_P01_getDataIdCRC(Config, Data);

    // Compute CRC over the area before the CRC (if CRC is not the first byte)
    if (crcByteIndex >= 1) {
        CRC = Crc_CalculateCRC8(Data, crcByteIndex, CRC, false);
    }

    // Compute the area after CRC, if CRC is not the last byte.
    // Start with the byte after CRC, finish with the last byte of data
    if (crcByteIndex < (dataLength - 1U)) {
        const size_t firstByteAfterCrc = crcByteIndex + 1;
        //const size_t remainingDataLength = dataLength - 2 - 1;
        CRC = Crc_CalculateCRC8(&Data[firstByteAfterCrc], (dataLength - crcByteIndex - 1), CRC, false);
    }

    CRC = CRC ^ 0xFF;

    return CRC;
}

static bool E2E_P01_CRCAndDataIDNibble(const E2E_P01ConfigType* Config,
                                       const uint8_t* Data) {
    // Read CRC from Data
    const size_t crcByteIndex = Config->CRCOffset / 8U;
    uint8_t ReceivedCRC = Data[crcByteIndex];

    bool dataIDNibbleOk = true;

    if (Config->DataIDMode == E2E_P01_DATAID_NIBBLE) {
        const uint8_t DataIDNibble = (Config->DataID >> 8) & 0x0F;

        uint8_t ReceivedDataIDNibble = 0;
        const size_t dataIDNibbleByteIndex = Config->DataIDNibbleOffset / 8U;
        // Read low nibble of high byte of Data ID from Data
        if ((Config->DataIDNibbleOffset % 8) == 0) {
            ReceivedDataIDNibble = Data[dataIDNibbleByteIndex] & 0x0F;
        } else {
            ReceivedDataIDNibble = (Data[dataIDNibbleByteIndex] >> 4U) & 0x0F;
        }

        dataIDNibbleOk = (ReceivedDataIDNibble == DataIDNibble);
    }

    uint8_t CalculatedCRC = E2E_P01_CalculateCRCOverDataIdAndData(Config, Data);

    return (ReceivedCRC == CalculatedCRC) && dataIDNibbleOk;
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
                              uint8_t* Data) {
    Std_ReturnType result = E2E_E_OK;

    if (Config == NULL || State == NULL || Data == NULL) {
        result = E2E_E_INPUTERR_NULL;
    } else if (configValid(Config) == false) {
        result = E2E_E_INPUTERR_WRONG;
    } else {
        writeCounter(Config, State, Data);

        writeDataIdNibble(Config, Data);

        const uint8_t crc = E2E_P01_CalculateCRCOverDataIdAndData(Config, Data);

        const size_t crcByteIndex = Config->CRCOffset / 8U;
        Data[crcByteIndex] = crc;

        incrementCounter(State);
    }


    return result;
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

    if ((Config == NULL) || (State == NULL) || (Data == NULL)) {
        result = E2E_E_INPUTERR_NULL;
    } else if (configValid(Config) == false) {
        result = E2E_E_INPUTERR_WRONG;
    } else {
        const uint8_t newDeltaCounter = State->MaxDeltaCounter + 1U;
        State->MaxDeltaCounter = uint8_min(newDeltaCounter, 14);

        if (State->NewDataAvailable) {
            uint8_t ReceivedCounter = readCounter(Config, Data);
            if (ReceivedCounter < 15) {
                bool dataValid = E2E_P01_CRCAndDataIDNibble(Config, Data);

                if (dataValid) {
                    if (State->WaitForFirstData) {
                        State->WaitForFirstData = false;
                        State->MaxDeltaCounter = Config->MaxDeltaCounterInit;
                        State->LastValidCounter = ReceivedCounter;
                        State->Status = E2E_P01STATUS_INITIAL;
                    } else {
                        State->Status = E2E_P01_process_counter(Config, State, ReceivedCounter);
                    }
                } else {
                    State->Status = E2E_P01STATUS_WRONGCRC;
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

    return result;
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
                    // Undefined, return error
                    result = E2E_P_ERROR;
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
                    // Undefined, return error
                    result = E2E_P_ERROR;
                    break;
            }
        }
    }



    return result;
}
