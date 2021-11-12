#ifndef E2ELIB_E2E_H__
#define E2ELIB_E2E_H__

/**
 * @file E2E.h
 * @author Tomas Wester (torsko@gmail.com)
 * @brief
 * @version 0.1
 * @date 2021-11-12
 *
 * @copyright Copyright (c) 2021
 *
 *
 * Reference documentation used for definitions:
 * [1] https://www.autosar.org/fileadmin/user_upload/standards/classic/20-11/AUTOSAR_SWS_E2ELibrary.pdf
  */

#include "Std_Types.h"


const Std_ReturnType E2E_E_OK = 0x00U;

/**
 * At least one pointer parameter is a NULL pointer
 *
 * Specified in [1] 7.1.1 Development Errors, SWS_E2E_00047
 */
const Std_ReturnType E2E_E_INPUTERR_NULL = 0x13U;

/**
 * At least one input parameter is erroneous, e.g. out of range
 *
 * Specified in [1] 7.1.1 Development Errors, SWS_E2E_00047
 */
const Std_ReturnType E2E_E_INPUTERR_WRONG = 0x17U;

/**
 * An internal library error has occurred (e.g. error detected by
 * program flow monitoring, violated invariant or postcondition)
 *
 * Specified in [1] 7.1.1 Development Errors, SWS_E2E_00047
 */
const Std_ReturnType E2E_E_INTERR = 0x19U;

/**
 * Function executed in wrong state
 *
 * Specified in [1] 7.1.1 Development Errors, SWS_E2E_00047
 */
const Std_ReturnType E2E_E_WRONGSTATE = 0x1AU;

/**
 * Returns the version information of this module.
 *
 * @param VersionInfo   Pointer to where to store the version information of this module.
 */
void E2E_GetVersionInfo(Std_VersionInfoType* VersionInfo);


#endif  // E2ELIB_E2E_H__
