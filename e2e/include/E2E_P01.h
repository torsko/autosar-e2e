#ifndef E2ELIB_E2E_P01_H__
#define E2ELIB_E2E_P01_H__

/**
 * @file E2E_P01.h
 * @author Tomas Wester (torsko@gmail.com)
 * @brief
 * @version 0.1
 * @date 2021-11-12
 *
 * @copyright Copyright (c) 2021
 *
 * Definitions specific to AUTOSAR E2E Profile 1
 *
 * Reference documentation used for implementation:
 *
 * [1] https://www.autosar.org/fileadmin/user_upload/standards/classic/20-11/AUTOSAR_SWS_E2ELibrary.pdf
 *
 * Implementation of E2E_P01_Forward is not included yet.
 *
 */

#include "E2E.h"
#include "E2E_SM.h"

/**
 * The Data ID is two bytes long in E2E Profile 1. There are four inclusion modes how
 * the implicit two-byte Data ID is included in the one-byte CRC.
 *
 * Specified in [1] 8.2.1.2 E2E_P01DataIDMode, SWS_E2E_00200
 */
typedef enum {
    /**
     * Two bytes are included in the CRC (double ID configuration) This
     * is used in E2E variant 1A.
     */
    E2E_P01_DATAID_BOTH     = 0,

    /**
     * One of the two bytes byte is included, alternating high and low
     * byte, depending on parity of the counter (alternating ID
     * configuration). For an even counter, the low byte is included. For
     * an odd counter, the high byte is included. This is used in E2E
     * variant 1B.
     */
    E2E_P01_DATAID_ALT      = 1,

    /**
     * Only the low byte is included, the high byte is never used. This is
     * applicable if the IDs in a particular system are 8 bits.
     */
    E2E_P01_DATAID_LOW      = 2,

    /**
     * The low byte is included in the implicit CRC calculation, the low
     * nibble of the high byte is transmitted along with the data (i.e. it is
     * explicitly included), the high nibble of the high byte is not used.
     * This is applicable for the IDs up to 12 bits. This is used in E2E
     * variant 1C.
     */
    E2E_P01_DATAID_NIBBLE   = 3
} E2E_P01DataIDMode;


/**
 * Configuration of transmitted Data (Data Element or I-PDU), for E2E Profile 1. For each
 * transmitted Data, there is an instance of this typedef.
 *
 * Specified in [1] 8.2.1.1 E2E_P01ConfigType, SWS_E2E_00018
 */
typedef struct {
    /**
     * Bit offset of Counter in MSB first order. CounterOffset shall be a multiple
     * of 4. In variants 1A, 1B, and 1C, CounterOffset is 8.
     */
    uint16_t CounterOffset;

    /**
     * Bit offset of CRC (i.e. since *Data) in MSB first order. The offset shall be
     * a multiple of 8. In variants 1A, 1B, and 1C, CRCOffset is 0.
     */
    uint16_t CRCOffset;

    /**
     * A unique identifier, for protection against masquerading. There are some
     * constraints on the selection of ID values, described in section
     * "Configuration constraints on Data IDs".
     */
    uint16_t DataID;

    /**
     * Bit offset of the low nibble of the high byte of Data ID. This parameter is
     * used by E2E Library only if DataIDMode = E2E_P01_DATAID_NIBBLE
     * (otherwise it is ignored by E2E Library).
     *
     * For DataIDMode different than E2E_P01_DATAID_NIBBLE, Data
     * IDNibbleOffset shall be initialized to 0 (even if it is ignored by E2E
     * Library).
     */
    uint16_t DataIDNibbleOffset;

    /**
     * Inclusion mode of ID in CRC computation (both bytes, alternating, or low
     * byte only of ID included).
     */
    E2E_P01DataIDMode DataIDMode;

    /**
     * Length of data, in bits. The value shall be a multiple of 8.
     */
    uint16_t DataLength;

    /**
     * Initial maximum allowed gap between two counter values of two
     * consecutively received valid Data. For example, if the receiver gets Data
     * with counter 1 and MaxDeltaCounterInit is 1, then at the next reception
     * the receiver can accept Counters with values 2 and 3, but not 4.
     * Note that if the receiver does not receive new Data at a consecutive read,
     * then the receiver increments the tolerance by 1.
     */
    uint8_t MaxDeltaCounterInit;

    /**
     * The maximum amount of missing or repeated Data which the receiver
     * does not expect to exceed under normal communication conditions.
     */
    uint8_t MaxNoNewOrRepeatedData;

    /**
     * Number of Data required for validating the consistency of the counter that
     * must be received with a valid counter (i.e. counter within the allowed lock-
     * in range) after the detection of an unexpected behavior of a received
     * counter.
     */
    uint8_t SyncCounterInit;
} E2E_P01ConfigType;


/**
 * State of the sender for a Data protected with E2E Profile 1.
 *
 * Specified in [1] 8.2.1.3 E2E_P01ProtectStateType, SWS_E2E_00020
 */
typedef struct {
    /**
     * Counter to be used for protecting the next Data. The initial value is 0,
     * which means that the first Data will have the counter 0. After the
     * protection by the Counter, the Counter is incremented modulo 0xF. The
     * value 0xF is skipped (after 0xE the next is 0x0), as 0xF value represents
     * the error value. The four high bits are always 0.
     */
    uint8_t Counter;
} E2E_P01ProtectStateType;


/**
 * Result of the verification of the Data in E2E Profile 1, determined by the Check
 * function.
 *
 * Specified in [1] 8.2.1.5 E2E_P01CheckStatusType, SWS_E2E_00022
 */
typedef enum {
    /**
     * OK: The new data has been received according to
     * communication medium, the CRC is correct, the
     * Counter is incremented by 1 with respect to the most
     * recent Data received with Status _INITIAL, _OK, or _
     * OKSOMELOST. This means that no Data has been
     * lost since the last correct data reception.
     */
    E2E_P01STATUS_OK = 0x00,

    /**
     * Error: the Check function has been invoked but no
     * new Data is not available since the last call,
     * according to communication medium (e.g. RTE,
     * COM). As a result, no E2E checks of Data have been
     * consequently executed.
     */
    E2E_P01STATUS_NONEWDATA = 0x01,

    /**
     * Error: The data has been received according to
     * communication medium, but 1. the CRC is incorrect
     * (applicable for all E2E Profile 1 configurations) or 2.
     * the low nibble of the high byte of Data ID is incorrect
     * (applicable only for E2E Profile 1 with E2E_P01Data
     * IDMode = E2E_P01_DATAID_NIBBLE).
     * The two above errors can be a result of corruption,
     * incorrect addressing or masquerade
     */
    E2E_P01STATUS_WRONGCRC = 0x02,

    /**
     * NOT VALID: The new data has been received after
     * detection of an unexpected behavior of counter. The
     * data has a correct CRC and a counter within the
     * expected range with respect to the most recent Data
     * received, but the determined continuity check for the
     * counter is not finalized yet.
     */
    E2E_P01STATUS_SYNC = 0x03,

    /**
     * Initial: The new data has been received according to
     * communication medium, the CRC is correct, but this
     * is the first Data since the receiver's initialization or
     * reinitialization, so the Counter cannot be verified yet.
     */
    E2E_P01STATUS_INITIAL = 0x04,

    /**
     * Error: The new data has been received according to
     * communication medium, the CRC is correct, but the
     * Counter is identical to the most recent Data received
     * with Status _INITIAL, _OK, or _OKSOMELOST.
     */
    E2E_P01STATUS_REPEATED = 0x08,

    /**
     * OK: The new data has been received according to
     * communication medium, the CRC is correct, the
     * Counter is incremented by DeltaCounter (1 < Delta
     * Counter = MaxDeltaCounter) with respect to the most
     * recent Data received with Status _INITIAL, _OK, or _
     * OKSOMELOST. This means that some Data in the
     * sequence have been probably lost since the last
     * correct/initial reception, but this is within the
     * configured tolerance range.
     */
    E2E_P01STATUS_OKSOMELOST = 0x20,

    /**
     * Error: The new data has been received according to
     * communication medium, the CRC is correct, but the
     * Counter Delta is too big (DeltaCounter > MaxDelta
     * Counter) with respect to the most recent Data
     * received with Status _INITIAL, _OK, or _
     * OKSOMELOST. This means that too many Data in
     * the sequence have been probably lost since the last
     * correct/initial reception.
     */
    E2E_P01STATUS_WRONGSEQUENCE = 0x40
} E2E_P01CheckStatusType;


/**
 * State of the receiver for a Data protected with E2E Profile 1
 *
 * Specified in [1] 8.2.1.4 E2E_P01CheckStateType, SWS_E2E_00021
 */
typedef struct {
    /**
     * Counter value most recently received. If no data has been yet received,
     * then the value is 0x0. After each reception, the counter is updated with
     * the value received.
     */
    uint8_t LastValidCounter;

    /**
     * MaxDeltaCounter specifies the maximum allowed difference between two
     * counter values of consecutively received valid messages
     */
    uint8_t MaxDeltaCounter;

    /**
     * If true means that no correct data (with correct Data ID and CRC) has
     * been yet received after the receiver initialization or reinitialization.
     */
    bool WaitForFirstData;

    /**
     * Indicates to E2E Library that a new data is available for Library to be
     * checked. This attribute is set by the E2E Library caller, and not by the
     * E2E Library.
     */
    bool NewDataAvailable;

    /**
     * Number of data (messages) lost since reception of last valid one. This
     * attribute is set only if Status equals E2E_P01STATUS_OK or E2E_
     * P01STATUS_OKSOMELOST. For other values of Status, the value of
     * LostData is undefined. E2E_P01CheckStatusType Status Result of the
     * verification of the Data, determined by the Check function.
     */
    uint8_t LostData;

    /**
     * Result of the verification of the Data, determined by the Check function.
     */
    E2E_P01CheckStatusType Status;

    /**
     * Number of Data required for validating the consistency of the counter that
     * must be received with a valid counter (i.e. counter within the allowed lock-
     * in range) after the detection of an unexpected behavior of a received
     * counter.
     */
    uint8_t SyncCounter;

    /**
     * Amount of consecutive reception cycles in which either (1) there was no
     * new data, or (2) when the data was repeated.
     */
    uint8_t NoNewOrRepeatedDataCounter;
} E2E_P01CheckStateType;

/**
 * Protects the array/buffer to be transmitted using the E2E profile 1. This includes
 * checksum calculation, handling of counter and Data ID.
 *
 * Specified in [1] 8.3.1.1 E2E_P01Protect, SWS_E2E_00166
 *
 * @param Config    Pointer to static configuration.
 * @param State     Pointer to port/data communication state.
 * @param Data      Pointer to Data to be transmitted.
 * @return Std_ReturnType
 * @retval E2E_E_INPUTERR_NULL
 * @retval E2E_E_INPUTERR_WRONG
 * @retval E2E_E_INTERR
 * @retval E2E_E_OK
 */
Std_ReturnType E2E_P01Protect(const E2E_P01ConfigType* Config,
                              E2E_P01ProtectStateType* State,
                              uint8_t* Data);


/**
 * Initializes the protection state
 *
 * Specified in [1] 8.3.1.2 E2E_P01ProtectInit, SWS_E2E_00385
 *
 * @param StatePtr  Pointer to port/data communication state
 * @return Std_ReturnType
 * @retval E2E_E_INPUTERR_NULL  Null pointer passed
 * @retval E2E_E_OK
 */
Std_ReturnType E2E_P01ProtectInit(E2E_P01ProtectStateType* State);


/**
 * Protects data which is forwarded by using the E2E profile 01. This includes
 * checksum calculation, handling of counter and Data ID. Detected Errors of received
 * message will be reconstruct on output data.
 * Tags:atp.Status=draft
 *
 * Specified in [1] 8.3.1.3 E2E_P01Forward, SWS_E2E_00588 {draft}
 *
 * @param ConfigPtr     Pointer to static configuration.
 * @param Checkstatus   E2E Status of the received message
 * @param StatePtr      Pointer to port/data communication state.
 * @param DataPtr       Pointer to Data to be transmitted.
 * @return Std_ReturnType
 * @retval E2E_E_INPUTERR_NULL
 * @retval E2E_E_INPUTERR_WRONG
 * @retval E2E_E_INTERR
 * @retval E2E_E_OK
 */
Std_ReturnType E2E_P01Forward(const E2E_P01ConfigType* ConfigPtr,
                              E2E_PCheckStatusType Checkstatus,
                              E2E_P01ProtectStateType* StatePtr,
                              uint8_t* DataPtr);


/**
 * Checks the Data received using the E2E profile 1. This includes CRC calculation,
 * handling of Counter and Data ID.
 *
 * Specified in [1] 8.3.1.4 E2E_P01Check, SWS_E2E_00158
 *
 * @note The State parameter is marked as inout in the specification
 *       indicating that the function may modify it.
 *
 * @param Config    Pointer to static configuration.
 * @param State     Pointer to port/data communication state.
 * @param Data      Pointer to received data
 * @return Std_ReturnType
 */
Std_ReturnType E2E_P01Check(const E2E_P01ConfigType* Config,
                            E2E_P01CheckStateType* State,
                            const uint8_t* Data);


/**
 * Initializes the check state
 *
 * @param StatePtr  Pointer to port/data communication state.
 * @return Std_ReturnType
 * @retval E2E_E_INPUTERR_NULL - null pointer passed
 * @retval E2E_E_OK
 */
Std_ReturnType E2E_P01CheckInit(E2E_P01CheckStateType* State);


/**
 * The function maps the check status of Profile 1 to a generic check status, which can
 * be used by E2E state machine check function. The E2E Profile 1 delivers a more
 * fine-granular status, but this is not relevant for the E2E state machine.
 *
 * Status mapping when profileBehavior is **TRUE**
 * | Status                         | Return value          |
 * |:-------------------------------|:----------------------|
 * | E2E_P01STATUS_OK               | E2E_P_OK              |
 * | E2E_P01STATUS_OKSOMELOST       | E2E_P_OK              |
 * | **E2E_P01STATUS_SYNC**         | E2E_P_OK              |
 * | E2E_P01STATUS_WRONGCRC         | E2E_P_ERROR           |
 * | E2E_P01STATUS_REPEATED         | E2E_P_REPEATED        |
 * | E2E_P01STATUS_NONEWDATA        | E2E_P_NONEWDATA       |
 * | E2E_P01STATUS_WRONGSEQUENCE    | E2E_P_WRONGSEQUENCE   |
 * | **E2E_P01STATUS_INITIAL**      | E2E_P_WRONGSEQUENCE   |
 *
 * Status mapping when profileBehavior is **FALSE**
 * | Status                         | Return value          |
 * |:-------------------------------|:----------------------|
 * | E2E_P01STATUS_OK               | E2E_P_OK              |
 * | E2E_P01STATUS_OKSOMELOST       | E2E_P_OK              |
 * | **E2E_P01STATUS_INITIAL**      | E2E_P_OK              |
 * | E2E_P01STATUS_WRONGCRC         | E2E_P_ERROR           |
 * | E2E_P01STATUS_REPEATED         | E2E_P_REPEATED        |
 * | E2E_P01STATUS_NONEWDATA        | E2E_P_NONEWDATA       |
 * | E2E_P01STATUS_WRONGSEQUENCE    | E2E_P_WRONGSEQUENCE   |
 * | **E2E_P01STATUS_SYNC**         | E2E_P_WRONGSEQUENCE   |
 *
 * @param CheckReturn       Return value of the E2E_P01Check function
 * @param Status            Status determined by E2E_P01Check function
 * @param profileBehavior   FALSE: check has the legacy behavior, before R4.2 TRUE: check behaves like new P4/P5/P6 profiles introduced in R4.2
 * @return E2E_PCheckStatusType     Profile-independent status of the reception on one single Data in one cycle
 */
E2E_PCheckStatusType E2E_P01MapStatusToSM (Std_ReturnType CheckReturn,
                                             E2E_P01CheckStatusType Status,
                                             bool profileBehavior);

#endif  // E2ELIB_E2E_P01_H__
