/*
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * Generate by template render, DONOT edit it
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 */



#ifndef __LIBINFO_H__
#define __LIBINFO_H__

#include "BootInfo.h"

#define FWVERSION_MAJOR                   4
#define FWVERSION_MINOR                   0
#define FWVERSION_REVISION                2
#define FWVERSION_BUILD                   0
#define LIB_INFO_FWVERSION                "4.0.2.0"


#define LIB_INFO_SOURCE_COMMIT_ID        "0c1a234cb2e78c8802e5c4b2fae32ce73d59ad8d"
#define LIB_INFO_CONFIG_COMMIT_ID        "0c1a234cb2e78c8802e5c4b2fae32ce73d59ad8d"
#define LIB_INFO_BUILD_TIME              "20220118181828"

#define LIB_INFO_BUILD_DATE_Y            2022
#define LIB_INFO_BUILD_DATE_M            1
#define LIB_INFO_BUILD_DATE_D            18

#define LIB_INFO_BUILD_TIME_H            18
#define LIB_INFO_BUILD_TIME_M            18
#define LIB_INFO_BUILD_TIME_S            28

#pragma push
#pragma CONST_SEG __GPAGE_SEG APP_INFO
extern const Boot_AppInfoTagType AppInfoTag;
extern boolean AppInfo_IsTestVersion(void);
#pragma pop
#endif
