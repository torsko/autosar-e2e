#ifndef E2E_STD_TYPES_H__
#define E2E_STD_TYPES_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/*
 As defined in https://www.autosar.org/fileadmin/user_upload/standards/classic/20-11/AUTOSAR_SWS_StandardTypes.pdf
*/

typedef uint8_t Std_ReturnType;
#define E_OK        0
#define E_NOT_OK    1


typedef struct {
    uint16_t vendorID;
    uint16_t moduleID;
    uint8_t sw_major_version;
    uint8_t sw_minor_version;
    uint8_t sw_patch_version;
} Std_VersionInfoType;


typedef uint8_t Std_MessageTypeType;
#define STD_MESSAGETYPE_REQUEST     0x00U
#define STD_MESSAGETYPE_RESPONSE    0x01U


typedef uint8_t Std_MessageResultType;
#define STD_MESSAGERESULT_OK        0x00U
#define STD_MESSAGERESULT_ERROR     0x01U







#endif  // E2E_STD_TYPES_H__
