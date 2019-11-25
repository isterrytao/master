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


#define LIB_INFO_SOURCE_COMMIT_ID        "4c9954cce1a8d89236b4468dd1168465d432cb04"
#define LIB_INFO_CONFIG_COMMIT_ID        "4c9954cce1a8d89236b4468dd1168465d432cb04"
#define LIB_INFO_BUILD_TIME              "20191125135904"

#define LIB_INFO_BUILD_DATE_Y            2019
#define LIB_INFO_BUILD_DATE_M            11
#define LIB_INFO_BUILD_DATE_D            25

#define LIB_INFO_BUILD_TIME_H            13
#define LIB_INFO_BUILD_TIME_M            59
#define LIB_INFO_BUILD_TIME_S            4

#pragma push
#pragma CONST_SEG __GPAGE_SEG APP_INFO
extern const Boot_AppInfoTagType AppInfoTag;
extern boolean AppInfo_IsTestVersion(void);
#pragma pop
#endif
