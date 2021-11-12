#ifndef E2ELIB_E2E_SM_H__
#define E2ELIB_E2E_SM_H__

/**
 * @file E2E_SM.h
 * @author Tomas Wester (torsko@gmail.com)
 * @brief
 * @version 0.1
 * @date 2021-11-12
 *
 * @copyright Copyright (c) 2021
 *
 * Reference documentation used for implementation:
 * [1] https://www.autosar.org/fileadmin/user_upload/standards/classic/20-11/AUTOSAR_SWS_E2ELibrary.pdf
 * [2] https://www.autosar.org/fileadmin/user_upload/standards/foundation/20-11/AUTOSAR_PRS_E2EProtocol.pdf
  */

#include "E2E.h"

/**
 * Profile-independent status of the reception on one single Data in one cycle.
 *
 * Specified in [1] 8.2.13.1 E2E_PCheckStatusType, SWS_E2E_00347
 */
typedef enum {
    /**
     * OK: the checks of the Data in this cycle were
     * successful (including counter check).
     */
    E2E_P_OK = 0x00,

    /**
     * Data has a repeated counter.
     */
    E2E_P_REPEATED = 0x01,

    /**
     * The checks of the Data in this cycle were
     * successful, with the exception of counter jump,
     * which changed more than the allowed delta.
     */
    E2E_P_WRONGSEQUENCE = 0x02,

    /**
     * Error not related to counters occurred (e.g. wrong
     * crc, wrong length, wrong Data ID) or the return of
     * the check function was not OK.
     */
    E2E_P_ERROR = 0x03,

    /**
     * No value has been received yet (e.g. during
     * initialization). This is used as the initialization
     * value for the buffer, it is not returned by any E2E
     * profile.
     */
    E2E_P_NOTAVAILABLE = 0x04,

    /**
     * No new data is available.
     */
    E2E_P_NONEWDATA = 0x05
} E2E_PCheckStatusType;


/**
 * Configuration of a communication channel for exchanging Data.
 *
 * Specified in [1] 8.2.13.2 E2E_SMConfigType, SWS_E2E_00342
 */
typedef struct {
    /**
     * Size of the monitoring window for the state machine during state VALID.
     */
    uint8_t WindowSizeValid;

    /**
     * Minimal number of checks in which ProfileStatus equal to E2E_P_OK
     * was determined within the last WindowSize checks (for the state E2E_
     * SM_INIT) required to change to state E2E_SM_VALID.
     */
    uint8_t MinOkStateInit;

    /**
     * Maximal number of checks in which ProfileStatus equal to E2E_P_
     * ERROR was determined, within the last WindowSize checks (for the
     * state E2E_SM_INIT).
     */
    uint8_t MaxErrorStateInit;

    /**
     * Minimal number of checks in which ProfileStatus equal to E2E_P_OK
     * was determined within the last WindowSize checks (for the state E2E_
     * SM_VALID) required to keep in state E2E_SM_VALID.
     */
    uint8_t MinOkStateValid;

    /**
     * Maximal number of checks in which ProfileStatus equal to E2E_P_
     * ERROR was determined, within the last WindowSize checks (for the
     * state E2E_SM_VALID).
     */
    uint8_t MaxErrorStateValid;

    /**
     * Minimum number of checks in which ProfileStatus equal to E2E_P_OK
     * was determined within the last WindowSize checks (for the state E2E_
     * SM_INVALID) required to change to state E2E_SM_VALID.
     */
    uint8_t MinOkStateInvalid;

    /**
     * Maximal number of checks in which ProfileStatus equal to E2E_P_
     * ERROR was determined, within the last WindowSize checks (for the
     * state E2E_SM_INVALID).
     */
    uint8_t MaxErrorStateInvalid;

    /**
     * Size of the monitoring windows for the state machine during state INIT.
     */
    uint8_t WindowSizeInit;

    /**
     * Size of the monitoring window for the state machine during state
     * INVALID.
     */
    uint8_t WindowSizeInvalid;

    /**
     * Clear monitoring window data on transition to state INVALID
     */
    bool ClearToInvalid;
} E2E_SMConfigType;


/**
 * Status of the communication channel exchanging the data. If the status is OK, then
 * the data may be used.
 *
 * Specified in [1] 8.2.13.4 E2E_SMStateType, SWS_E2E_00344
 */
typedef enum {
    /**
     * Communication functioning properly according to E2E,
     * data can be used.
     */
    E2E_SM_VALID = 0x00,

    /**
     * State before E2E_SMCheckInit() is invoked, data cannot
     * be used.
     */
    E2E_SM_DEINIT = 0x00,

    /**
     * No data from the sender is available since the initialization,
     * data cannot be used.
     */
    E2E_SM_NODATA = 0x00,

    /**
     * There has been some data received since startup, but it is
     * not yet possible use it, data cannot be used
     */
    E2E_SM_INIT = 0x00,

    /**
     * Communication not functioning properly, data cannot be
     * used.
     */
    E2E_SM_INVALID = 0x00,
} E2E_SMStateType;


/**
 * State of the protection of a communication channel.
 *
 * Specified in [1] 8.2.13.3 E2E_SMCheckStateType, SWS_E2E_00343
 */
typedef struct {
    /**
     * Pointer to an array, in which the ProfileStatus-es of the last E2E-checks
     * are stored.
     * The array size shall be WindowSize
     *
     * @note Reading the description in [2], it is understood that the
     * corresponding WindowSize configuration parameter to use is
     * **WindowSizeValid**
     */
    uint8_t* ProfileStatusWindow;

    /**
     * index in the array, at which the next ProfileStatus is to be written.
     */
    uint8_t WindowTopIndex;

    /**
     * Count of checks in which ProfileStatus equal to E2E_P_OK was
     * determined, within the last WindowSize checks.
     */
    uint8_t OkCount;

    /**
     * Count of checks in which ProfileStatus equal to E2E_P_ERROR was
     * determined, within the last WindowSize checks
     */
    uint8_t ErrorCount;

    /**
     * The current state in the state machine. The value is not explicitly used in
     * the pseudocode of the state machine, because it is expressed in UML as
     * UML states.
     */
    E2E_SMStateType SMState;
} E2E_SMCheckStateType;


/**
 * Checks the communication channel. It determines if the data can be used for
 * safety-related application, based on history of checks performed by a corresponding
 * E2E_P0XCheck() function.
 *
 * @param ProfileStatus Profile-independent status of the reception on one single Data in one cycle
 * @param ConfigPtr     Pointer to static configuration
 * @param StatePtr      Pointer to port/data communication state
 * @return Std_ReturnType
 * @retval E2E_E_INPUTERR_NULL
 * @retval E2E_E_INPUTERR_WRONG
 * @retval E2E_E_INTERR
 * @retval E2E_E_OK
 * @retval E2E_E_WRONGSTATE
 */
Std_ReturnType E2E_SMCheck(E2E_PCheckStatusType ProfileStatus,
                           const E2E_SMConfigType* ConfigPtr,
                           E2E_SMCheckStateType* StatePtr);


/**
 * Initializes the state machine
 *
 * @note StatePtr is specified as an out parameter.
 *
 * @param StatePtr  Pointer to port/data communication state.
 * @param ConfigPtr Pointer to configuration of the state machine
 * @return Std_ReturnType
 * @retval E2E_E_INPUTERR_NULL
 * @retval E2E_E_OK
 */
Std_ReturnType E2E_SMCheckInit(E2E_SMCheckStateType* StatePtr,
                               const E2E_SMConfigType* ConfigPtr);



#endif  // E2ELIB_E2E_SM_H__
