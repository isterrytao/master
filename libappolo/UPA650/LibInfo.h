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
#define FWVERSION_REVISION                4
#define FWVERSION_BUILD                   4
#define LIB_INFO_FWVERSION                "3.0.4.4"


#define LIB_INFO_SOURCE_COMMIT_ID        "afa43efdd9a173b5e2af1817f8332b4dcb422b97"
#define LIB_INFO_CONFIG_COMMIT_ID        "afa43efdd9a173b5e2af1817f8332b4dcb422b97"
#define LIB_INFO_BUILD_TIME              "20191216105812"

#define LIB_INFO_BUILD_DATE_Y            2019
#define LIB_INFO_BUILD_DATE_M            12
#define LIB_INFO_BUILD_DATE_D            16

#define LIB_INFO_BUILD_TIME_H            10
#define LIB_INFO_BUILD_TIME_M            58
#define LIB_INFO_BUILD_TIME_S            12

#pragma push
#pragma CONST_SEG __GPAGE_SEG APP_INFO
extern const Boot_AppInfoTagType AppInfoTag;
extern boolean AppInfo_IsTestVersion(void);
#pragma pop
#endif
