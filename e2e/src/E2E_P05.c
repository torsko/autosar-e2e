#include "E2E_P05.h"

#include "Crc.h"

static Std_ReturnType protectParametersValid(const E2E_P05ConfigType* Config,
                                             const E2E_P05ProtectStateType* State,
                                             const uint8_t* Data,
                                             uint16_t Length) {
    Std_ReturnType result = E2E_E_OK;

    if ((Config == NULL) || (State == NULL) || (Data == NULL)) {
        result = E2E_E_INPUTERR_NULL;
    } else if (Length != (Config->DataLength / 8U)) {
        result = E2E_E_INPUTERR_WRONG;
    } else if ((Config->Offset % 8) != 0) {
        result = E2E_E_INPUTERR_WRONG;
    } else if ((Config->DataLength < (3*8)) || (Config->DataLength > (4096*8))) {
        result = E2E_E_INPUTERR_WRONG;
    } else if (Config->Offset > (Config->DataLength - 3*8)) {
        result = E2E_E_INPUTERR_WRONG;
    }

    return result;
}

Std_ReturnType E2E_P05ProtectInit(E2E_P05ProtectStateType* State) {
    Std_ReturnType result = E2E_E_OK;
    if (State == NULL) {
        result = E2E_E_INPUTERR_NULL;
    } else {
        State->Counter = 0;
    }

    return result;
}

Std_ReturnType E2E_P05Protect(const E2E_P05ConfigType* Config,
                              E2E_P05ProtectStateType* State,
                              uint8_t* Data,
                              uint16_t Length) {

    Std_ReturnType result = protectParametersValid(Config, State, Data, Length);

    if (result == E2E_E_OK) {
        const uint16_t Offset = Config->Offset / 8U;
        const uint16_t CounterOffset = Offset + 2U;
        const uint16_t firstDataAfterCRCByteIndex = Offset + 2U;

        // Write Counter
        Data[CounterOffset] = State->Counter;

        // Compute CRC
        uint16_t ComputedCRC = 0;
        if (Config->Offset > 0) {
            // Calculate CRC for parts before CRC position
            ComputedCRC = Crc_CalculateCRC16(Data, Offset, 0xFFFF, true);

            // Calculate CRC for parts after CRC position
            const uint16_t dataLengthAfterCrc = Length - Offset - 2U;
            ComputedCRC = Crc_CalculateCRC16(&Data[firstDataAfterCRCByteIndex], dataLengthAfterCrc, ComputedCRC, false);

        } else {
            // Calculate CRC over data

            ComputedCRC = Crc_CalculateCRC16(&Data[firstDataAfterCRCByteIndex], (Length - 2U), 0xFFFF, true);
        }

        // Calculate CRC for DataID
        const uint8_t dataIDLSB = Config->DataID & 0xFF;
        const uint8_t dataIDMSB = (Config->DataID >> 8U) & 0xFF;

        ComputedCRC = Crc_CalculateCRC16(&dataIDLSB, 1, ComputedCRC, false);
        ComputedCRC = Crc_CalculateCRC16(&dataIDMSB, 1, ComputedCRC, false);

        // Write CRC
        const uint8_t computedCRCLSB = ComputedCRC & 0xFF;
        const uint8_t computedCRCMSB = (ComputedCRC >> 8) & 0xFF;
        const uint16_t crcLSBByteIndex = Offset;
        const uint16_t crcMSBByteIndex = Offset + 1;

        Data[crcLSBByteIndex] = computedCRCLSB;
        Data[crcMSBByteIndex] = computedCRCMSB;

        // Increment Counter
        if (State->Counter == 0xFF) {
            State->Counter = 0;
        } else {
            State->Counter += 1;
        }
    }

    return result;
}

E2E_PCheckStatusType E2E_P05MapStatusToSM(Std_ReturnType CheckReturn,
                                          E2E_P05CheckStatusType Status) {

    E2E_PCheckStatusType result = E2E_P_OK;

    if (CheckReturn != E2E_E_OK) {
        result = E2E_P_ERROR;
    } else {
        switch (Status) {
            case E2E_P05STATUS_OK:
                // Intentional fall-through
            case E2E_P05STATUS_OKSOMELOST:
                result = E2E_P_OK;
                break;
            case E2E_P05STATUS_ERROR:
                result = E2E_P_ERROR;
                break;
            case E2E_P05STATUS_REPEATED:
                result = E2E_P_REPEATED;
                break;
            case E2E_P05STATUS_NONEWDATA:
                result = E2E_P_NONEWDATA;
                break;
            case E2E_P05STATUS_WRONGSEQUENCE:
                result = E2E_P_WRONGSEQUENCE;
                break;

            default:
                // Undefined, return error
                result = E2E_P_ERROR;
                break;
        }
    }

    return result;
}
