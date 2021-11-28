#ifndef E2ELIB_E2E_P05_H__
#define E2ELIB_E2E_P05_H__

/**
 * @file E2E_P05.h
 * @author Tomas Wester (torsko@gmail.com)
 * @brief
 * @version 0.1
 * @date 2021-11-22
 *
 * @copyright Copyright (c) 2021
 *
 * Definitions specific to AUTOSAR E2E Profile 5
 *
 * Reference documentation used for implementation:
 *
 * [1] https://www.autosar.org/fileadmin/user_upload/standards/classic/20-11/AUTOSAR_SWS_E2ELibrary.pdf
 *
 * Implementation of E2E_P05Forward is not included yet.
 *
 */

#include "E2E.h"
#include "E2E_SM.h"

/**
 * Configuration of transmitted Data (Data Element or I-PDU), for E2E Profile 5. For each
 * transmitted Data, there is an instance of this typedef.
 *
 * Specified in [1] 8.2.5.1 E2E_P05ConfigType, SWS_E2E_00437
 */
typedef struct {
    /**
     * Bit offset of the first bit of the E2E header from the beginning of the Data
     * (bit numbering: bit 0 is the least important). The offset shall be a multiple
     * of 8 and 0 <= Offset <= DataLength-(3*8). Example: If Offset equals 8,
     * then the low byte of the E2E Crc (16 bit) is written to Byte 1, the high Byte
     * is written to Byte 2.
     */
    uint16_t Offset;


    /**
     * Length of data, in bits.
     */
    uint16_t DataLength;

    /**
     * A system-unique identifier of the Data
     */
    uint16_t DataID;

    /**
     * Maximum allowed gap between two counter values of two consecutively
     * received valid Data. For example, if the receiver gets Data with counter 1
     * and MaxDeltaCounter is 3, then at the next reception the receiver can
     * accept Counters with values 2, 3 or 4.
     */
    uint8_t MaxDeltaCounter;
} E2E_P05ConfigType;


/**
 * State of the sender for a Data protected with E2E Profile 5.
 *
 * Specified in [1] 8.2.5.2 E2E_P05ProtectStateType, SWS_E2E_00438
 */
typedef struct {
    /**
     * Counter to be used for protecting the next Data. The initial value is 0,
     * which means that in the first cycle, Counter is 0. Each time E2E_
     * P05Protect() is called, it increments the counter up to 0xFF.
     */
    uint8_t Counter;
} E2E_P05ProtectStateType;


/**
 * Status of the reception on one single Data in one cycle, protected with E2E Profile 5.
 *
 * Specified in [1] 8.2.5.4 E2E_P01CheckStatusType, SWS_E2E_00440
 */
typedef enum {
    /**
     * OK: the checks of the Data in this cycle were
     * successful (including counter check, which was
     * incremented by 1).
     */
    E2E_P05STATUS_OK = 0x00,

    /**
     * Error: the Check function has been invoked but no
     * new Data is not available since the last call,
     * according to communication medium (e.g. RTE,
     * COM). As a result, no E2E checks of Data have
     * been consequently executed.
     * This may be considered similar to E2E_
     * P05STATUS_REPEATED.
     */
    E2E_P05STATUS_NONEWDATA = 0x01,

    /**
     * Error: error not related to counters occurred (e.g.
     * wrong crc, wrong length).
     */
    E2E_P05STATUS_ERROR = 0x07,

    /**
     * Error: the checks of the Data in this cycle were
     * successful, with the exception of the repetition.
     */
    E2E_P05STATUS_REPEATED = 0x08,

    /**
     * OK: the checks of the Data in this cycle were
     * successful (including counter check, which was
     * incremented within the allowed configured delta).
     */
    E2E_P05STATUS_OKSOMELOST = 0x20,

    /**
     * Error: the checks of the Data in this cycle were
     * successful, with the exception of counter jump,
     * which changed more than the allowed delta
     */
    E2E_P05STATUS_WRONGSEQUENCE = 0x40
} E2E_P05CheckStatusType;


/**
 * State of the reception on one single Data protected with E2E Profile 5.
 *
 * Specified in [1] 8.2.5.3 E2E_P05CheckStateType, SWS_E2E_00439
 */
typedef struct {
    /**
     * Result of the verification of the Data in this cycle, determined by the
     * Check function.
     */
    E2E_P05CheckStatusType Status;

    /**
     * Counter of the data in previous cycle
     */
    uint8_t Counter;
} E2E_P05CheckStateType;

/**
 * Protects the array/buffer to be transmitted using the E2E profile 5. This includes
 * checksum calculation, handling of counter.
 *
 * Specified in [1] 8.3.5.1 E2E_P05Protect, SWS_E2E_00446
 *
 * @param Config    Pointer to static configuration.
 * @param State     Pointer to port/data communication state.
 * @param Data      Pointer to Data to be transmitted.
 * @param Length    Length of the data in bytes
 * @return Std_ReturnType
 * @retval E2E_E_INPUTERR_NULL
 * @retval E2E_E_INPUTERR_WRONG
 * @retval E2E_E_INTERR
 * @retval E2E_E_OK
 */
Std_ReturnType E2E_P05Protect(const E2E_P05ConfigType* Config,
                              E2E_P05ProtectStateType* State,
                              uint8_t* Data,
                              uint16_t Length);


/**
 * Initializes the protection state
 *
 * Specified in [1] 8.3.5.2 E2E_P01ProtectInit, SWS_E2E_00447
 *
 * @param StatePtr  Pointer to port/data communication state
 * @return Std_ReturnType
 * @retval E2E_E_INPUTERR_NULL  Null pointer passed
 * @retval E2E_E_OK
 */
Std_ReturnType E2E_P05ProtectInit(E2E_P05ProtectStateType* State);


/**
 * Protects data which is forwarded by using the E2E profile 05. This includes
 * checksum calculation, handling of counter and Data ID. Detected Errors of received
 * message will be reconstruct on output data.
 * Tags:atp.Status=draft
 *
 * Specified in [1] 8.3.5.3 E2E_P05Forward, SWS_E2E_00585 {draft}
 *
 * @note Not yet implemented!
 *
 * @param ConfigPtr     Pointer to static configuration.
 * @param Length        Length of the data in bytes.
 * @param Checkstatus   E2E Status of the received message
 * @param StatePtr      Pointer to port/data communication state.
 * @param DataPtr       Pointer to Data to be transmitted.
 * @return Std_ReturnType
 * @retval E2E_E_INPUTERR_NULL
 * @retval E2E_E_INPUTERR_WRONG
 * @retval E2E_E_INTERR
 * @retval E2E_E_OK
 */
Std_ReturnType E2E_P05Forward(const E2E_P05ConfigType* ConfigPtr,
                              uint16_t Length,
                              E2E_PCheckStatusType Checkstatus,
                              E2E_P05ProtectStateType* StatePtr,
                              uint8_t* DataPtr);


/**
 * Checks the Data received using the E2E profile 5. This includes CRC calculation,
 * handling of Counter.
 * The function checks only one single data in one cycle, it does not determine/
 * compute the accumulated state of the communication link.
 *
 * Specified in [1] 8.3.5.4 E2E_P05Check, SWS_E2E_00449
 *
 * @param Config    Pointer to static configuration.
 * @param State     Pointer to port/data communication state.
 * @param Data      Pointer to received data
 * @param Length    Length of the data in bytes.
 * @return Std_ReturnType
 * @retval E2E_E_INPUTERR_NULL
 * @retval E2E_E_INPUTERR_WRONG
 * @retval E2E_E_INTERR
 * @retval E2E_E_OK
 */
Std_ReturnType E2E_P05Check(const E2E_P05ConfigType* Config,
                            E2E_P05CheckStateType* State,
                            const uint8_t* Data,
                            uint16_t Length);


/**
 * Initializes the check state
 *
 * Specified in [1] 8.3.5.5 E2E_P05CheckInit, SWS_E2E_00450
 *
 * @param StatePtr  Pointer to port/data communication state.
 * @return Std_ReturnType
 * @retval E2E_E_INPUTERR_NULL - null pointer passed
 * @retval E2E_E_OK
 */
Std_ReturnType E2E_P05CheckInit(E2E_P05CheckStateType* State);


/**
 * The function maps the check status of Profile 5 to a generic check status, which can
 * be used by E2E state machine check function. The E2E Profile 5 delivers a more
 * fine-granular status, but this is not relevant for the E2E state machine.
 *
 * Specified in [1] 8.3.5.6 E2E_P05MapStatusToSM, SWS_E2E_00452
 *
 * | Status                         | Return value          |
 * |:-------------------------------|:----------------------|
 * | E2E_P05STATUS_OK               | E2E_P_OK              |
 * | E2E_P05STATUS_OKSOMELOST       | E2E_P_OK              |
 * | E2E_P05STATUS_ERROR            | E2E_P_ERROR           |
 * | E2E_P05STATUS_REPEATED         | E2E_P_REPEATED        |
 * | E2E_P05STATUS_NONEWDATA        | E2E_P_NONEWDATA       |
 * | E2E_P05STATUS_WRONGSEQUENCE    | E2E_P_WRONGSEQUENCE   |
 *
 * @param CheckReturn       Return value of the E2E_P05Check function
 * @param Status            Status determined by E2E_P05Check function
 * @return E2E_PCheckStatusType     Profile-independent status of the reception on one single Data in one cycle
 */
E2E_PCheckStatusType E2E_P05MapStatusToSM(Std_ReturnType CheckReturn,
                                          E2E_P05CheckStatusType Status);

#endif  // E2ELIB_E2E_P01_H__
