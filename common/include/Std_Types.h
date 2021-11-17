#ifndef E2E_STD_TYPES_H__
#define E2E_STD_TYPES_H__

/**
 * @file Std_Types.h
 * @author Tomas Wester (torsko@gmail.com)
 * @brief
 * @version 0.1
 * @date 2021-11-12
 *
 * @copyright Copyright (c) 2021
 *
 * The types defined in this header file are implemented according to the listed reference documentation.
 *
 * Reference documentation used for implementation:
 * [1] https://www.autosar.org/fileadmin/user_upload/standards/classic/20-11/AUTOSAR_SWS_StandardTypes.pdf
 *
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>




/**
 * This type can be used as standard API return type which is shared between the RTE
 * and the BSW modules. It shall be defined as follows:
 * typedef uint8 Std_ReturnType;
 *
 * Specified in [1] 8.1.1 Std_ReturnType, SWS_Std_00005
 */
typedef uint8_t Std_ReturnType;

/**
 * This type shall be used to request the version of a BSW module using the <Module
 * name>_GetVersionInfo() function.
 *
 * Specified in [1] 8.1.2 Std_VersionInfoType, SWS_Std_00015
 */
typedef struct {
    uint16_t vendorID;
    uint16_t moduleID;
    uint8_t sw_major_version;
    uint8_t sw_minor_version;
    uint8_t sw_patch_version;
} Std_VersionInfoType;

/**
 * This type is used to encode the different type of messages. - Currently this encoding
 * is limited to the distinction between requests and responses in C/S communication.
 *
 * Specified in [1] 8.1.5 Std_MessageTypeType, SWS_Std_91001
 */
typedef uint8_t Std_MessageTypeType;

/**
 * Message type for a request message
 */
#define STD_MESSAGETYPE_REQUEST     0x00U

/**
 * Message type for a response message
 */
#define STD_MESSAGETYPE_RESPONSE    0x01U


/**
 * This type is used to encode different types of results for response messages. -
 * Currently this encoding is limited to the distinction between OK and ERROR
 * responses.
 *
 * Specified in [1] 8.1.6 Std_MessageResultType, SWS_Std_91002
 */
typedef uint8_t Std_MessageResultType;

#define STD_MESSAGERESULT_OK        0x00U

/**
 * Messageresult for an ERROR response
 */
#define STD_MESSAGERESULT_ERROR     0x01U


/**
 * Because E_OK is already defined within OSEK, the symbol E_OK has to be shared.
 * To avoid name clashes and redefinition problems, the symbols have to be defined in
 * the following way (approved within implementation):
 * <sample code from description omitted, seen below>
 *
 * Specified in [1] 8.2.1 E_OK, E_NOT_OK, SWS_Std_00006
 */
#ifndef STATUSTYPEDEFINED
#define STATUSTYPEDEFINED
#define E_OK 0x00u
typedef unsigned char StatusType; /* OSEK compliance */
#endif
#define E_NOT_OK 0x01u









#endif  // E2E_STD_TYPES_H__
