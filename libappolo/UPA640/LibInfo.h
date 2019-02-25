/*
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * Generate by template render, DONOT edit it
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 */



#ifndef __LIBINFO_H__
#define __LIBINFO_H__

#include "BootInfo.h"

#define FWVERSION_MAJOR                   3
#define FWVERSION_MINOR                   0
#define FWVERSION_REVISION                3
#define FWVERSION_BUILD                   1
#define LIB_INFO_FWVERSION                "3.0.3.21"


#define LIB_INFO_SOURCE_COMMIT_ID        "db80934351b4d71e0c4dcf80ec26f1d19b377fc6"
#define LIB_INFO_CONFIG_COMMIT_ID        "db80934351b4d71e0c4dcf80ec26f1d19b377fc6"
#define LIB_INFO_BUILD_TIME              "20190225102917"

#define LIB_INFO_BUILD_DATE_Y            2019
#define LIB_INFO_BUILD_DATE_M            2
#define LIB_INFO_BUILD_DATE_D            25

#define LIB_INFO_BUILD_TIME_H            10
#define LIB_INFO_BUILD_TIME_M            29
#define LIB_INFO_BUILD_TIME_S            17

#pragma push
#pragma CONST_SEG __GPAGE_SEG APP_INFO
extern const Boot_AppInfoTagType AppInfoTag;
extern boolean AppInfo_IsTestVersion(void);
#pragma pop
#endif
