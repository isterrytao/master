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
#define FWVERSION_REVISION                1
#define FWVERSION_BUILD                   0
#define LIB_INFO_FWVERSION                "4.0.1.0"


#define LIB_INFO_SOURCE_COMMIT_ID        "38a747c75100bc0fc82525d8a3db187e90b772b9"
#define LIB_INFO_CONFIG_COMMIT_ID        "38a747c75100bc0fc82525d8a3db187e90b772b9"
#define LIB_INFO_BUILD_TIME              "20211223153015"

#define LIB_INFO_BUILD_DATE_Y            2021
#define LIB_INFO_BUILD_DATE_M            12
#define LIB_INFO_BUILD_DATE_D            23

#define LIB_INFO_BUILD_TIME_H            15
#define LIB_INFO_BUILD_TIME_M            30
#define LIB_INFO_BUILD_TIME_S            15

#pragma push
#pragma CONST_SEG __GPAGE_SEG APP_INFO
extern const Boot_AppInfoTagType AppInfoTag;
extern boolean AppInfo_IsTestVersion(void);
#pragma pop
#endif
