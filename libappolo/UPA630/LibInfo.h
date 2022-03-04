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


#define LIB_INFO_SOURCE_COMMIT_ID        "003847f02dc5daf7dabe39fac352a4b90280d132"
#define LIB_INFO_CONFIG_COMMIT_ID        "003847f02dc5daf7dabe39fac352a4b90280d132"
#define LIB_INFO_BUILD_TIME              "20220304201440"

#define LIB_INFO_BUILD_DATE_Y            2022
#define LIB_INFO_BUILD_DATE_M            3
#define LIB_INFO_BUILD_DATE_D            4

#define LIB_INFO_BUILD_TIME_H            20
#define LIB_INFO_BUILD_TIME_M            14
#define LIB_INFO_BUILD_TIME_S            40

#pragma push
#pragma CONST_SEG __GPAGE_SEG APP_INFO
extern const Boot_AppInfoTagType AppInfoTag;
extern boolean AppInfo_IsTestVersion(void);
#pragma pop
#endif
