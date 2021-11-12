#ifndef E2ELIB_E2E_H__
#define E2ELIB_E2E_H__

#include "Std_Types.h"

// Error flags
#define E2E_E_INPUTERR_NULL     0x13U
#define E2E_E_INPUTERR_WRONG    0x17U
#define E2E_E_INTERR            0x19U
#define E2E_E_WRONGSTATE        0x1AU
#define E2E_E_OK                0x00U

// E2E_PCheckStatusType
// Profile-independent status of the reception on one single Data in one cycle.
typedef enum {
    E2E_P_OK = 0x00,
    E2E_P_REPEATED = 0x01,
    E2E_P_WRONGSEQUENCE = 0x02,
    E2E_P_ERROR = 0x03,
    E2E_P_NOTAVAILABLE = 0x04,
    E2E_P_NONEWDATA = 0x05
} E2E_PCheckStatusType;

//=========================== P01 BEGINS ===========================//

// E2E_P01DataIDMode
// The Data ID is two bytes long in E2E Profile 1. There are four inclusion modes how
// the implicit two-byte Data ID is included in the one-byte CRC.
typedef enum {
    E2E_P01_DATAID_BOTH = 0,
    E2E_P01_DATAID_ALT = 1,
    E2E_P01_DATAID_LOW = 2,
    E2E_P01_DATAID_NIBBLE = 3
} E2E_P01DataIDMode;

// E2E_P01ConfigType
// Configuration of transmitted Data (Data Element or I-PDU), for E2E Profile 1. For each
// transmitted Data, there is an instance of this typedef.
typedef struct {
    // Bit offset of Counter in MSB first order.
    // CounterOffset shall be a multiple of 4.
    // In variants 1A, 1B, and 1C, CounterOffset is 8.
    uint16_t CounterOffset;

    // Bit offset of CRC (i.e. since *Data) in MSB first order.
    // The offset shall be a multiple of 8.
    // In variants 1A, 1B, and 1C, CRCOffset is 0.
    uint16_t CRCOffset;

    // A unique identifier, for protection against masquerading.
    // There are some constraints on the selection of ID values,
    // described in section  "Configuration constraints on Data IDs"
    uint16_t DataID;

    // Bit offset of the low nibble of the high byte of Data ID.
    // This parameter is used by E2E Library only if DataIDMode = E2E_P01_DATAID_NIBBLE
    // (otherwise it is ignored by E2E Library).
    // For DataIDMode different than E2E_P01_DATAID_NIBBLE, DataIDNibbleOffset
    // shall be initialized to 0 (even if it is ignored by E2E Library).
    uint16_t DataIDNibbleOffset;

    // Inclusion mode of ID in CRC computation (both bytes, alternating, or low
    // byte only of ID included).
    E2E_P01DataIDMode DataIDMode;

    // Length of data, in bits. The value shall be a multiple of 8.
    uint16_t DataLength;

    // Initial maximum allowed gap between two counter values of two
    // consecutively received valid Data. For example, if the receiver gets Data
    // with counter 1 and MaxDeltaCounterInit is 1, then at the next reception
    // the receiver can accept Counters with values 2 and 3, but not 4.
    // Note that if the receiver does not receive new Data at a consecutive read,
    // then the receiver increments the tolerance by 1.
    uint8_t MaxDeltaCounterInit;

    // The maximum amount of missing or repeated Data which the receiver
    // does not expect to exceed under normal communication conditions.
    uint8_t MaxNoNewOrRepeatedData;

    // Number of Data required for validating the consistency of the counter that
    // must be received with a valid counter (i.e. counter within the allowed lock-
    // in range) after the detection of an unexpected behavior of a received
    // counter.
    uint8_t SyncCounterInit;

} E2E_P01ConfigType;


// E2E_P01ProtectStateType
// State of the sender for a Data protected with E2E Profile 1.
typedef struct {
    uint8_t Counter;
} E2E_P01ProtectStateType;


// E2E_P01CheckStatusType
// Result of the verification of the Data in E2E Profile 1, determined by the Check
// function.
typedef enum {
    E2E_P01STATUS_OK = 0x00,
    E2E_P01STATUS_NONEWDATA = 0x01,
    E2E_P01STATUS_WRONGCRC = 0x02,
    E2E_P01STATUS_SYNC = 0x03,
    E2E_P01STATUS_INITIAL = 0x04,
    E2E_P01STATUS_REPEATED = 0x08,
    E2E_P01STATUS_OKSOMELOST = 0x20,
    E2E_P01STATUS_WRONGSEQUENCE = 0x40
} E2E_P01CheckStatusType;


// E2E_P01CheckStateType
// State of the receiver for a Data protected with E2E Profile 1.
typedef struct {
    // Counter value most recently received. If no data has been yet received,
    // then the value is 0x0. After each reception, the counter is updated with
    // the value received.
    uint8_t LastValidCounter;

    // MaxDeltaCounter specifies the maximum allowed difference between two
    // counter values of consecutively received valid messages
    uint8_t MaxDeltaCounter;

    // If true means that no correct data (with correct Data ID and CRC) has
    // been yet received after the receiver initialization or reinitialization.
    bool WaitForFirstData;

    // Indicates to E2E Library that a new data is available for Library to be
    // checked. This attribute is set by the E2E Library caller, and not by the
    // E2E Library.
    bool NewDataAvailable;

    // Number of data (messages) lost since reception of last valid one. This
    // attribute is set only if Status equals E2E_P01STATUS_OK or E2E_
    // P01STATUS_OKSOMELOST. For other values of Status, the value of
    // LostData is undefined. E2E_P01CheckStatusType Status Result of the
    // verification of the Data, determined by the Check function.
    uint8_t LostData;

    // Result of the verification of the Data, determined by the Check function.
    E2E_P01CheckStatusType Status;

    // Number of Data required for validating the consistency of the counter that
    // must be received with a valid counter (i.e. counter within the allowed lock-
    // in range) after the detection of an unexpected behavior of a received
    // counter.
    uint8_t SyncCounter;

    // Amount of consecutive reception cycles in which either (1) there was no
    // new data, or (2) when the data was repeated.
    uint8_t NoNewOrRepeatedDataCounter;
} E2E_P01CheckStateType;

// Protects the array/buffer to be transmitted using the E2E profile 1. This includes
// checksum calculation, handling of counter and Data ID
Std_ReturnType E2E_P01Protect(const E2E_P01ConfigType* ConfigPtr,
                              E2E_P01ProtectStateType* StatePtr,
                              uint8_t* DataPtr);

// Initializes the protection state
Std_ReturnType E2E_P01ProtectInit(E2E_P01ProtectStateType* StatePtr);

// Protects data which is forwarded by using the E2E profile 01. This includes
// checksum calculation, handling of counter and Data ID. Detected Errors of received
// message will be reconstruct on output data.
// Tags:atp.Status=draft
Std_ReturnType E2E_P01Forward(const E2E_P01ConfigType* ConfigPtr,
                              E2E_PCheckStatusType Checkstatus,
                              E2E_P01ProtectStateType* StatePtr,
                              uint8_t* DataPtr);

// Checks the Data received using the E2E profile 1. This includes CRC calculation,
// handling of Counter and Data ID.
Std_ReturnType E2E_P01Check(const E2E_P01ConfigType* Config,
                            E2E_P01CheckStateType* State,
                            const uint8_t* Data);

// Initializes the check state
Std_ReturnType E2E_P01CheckInit(E2E_P01CheckStateType* StatePtr);

// The function maps the check status of Profile 1 to a generic check status, which can
// be used by E2E state machine check function. The E2E Profile 1 delivers a more
// fine-granular status, but this is not relevant for the E2E state machine.
E2E_PCheckStatusType E2E_P01MapStatusToSM (Std_ReturnType CheckReturn,
                                             E2E_P01CheckStatusType Status,
                                             bool profileBehavior);

#endif  // E2ELIB_E2E_H__
