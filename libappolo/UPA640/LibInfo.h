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
#define FWVERSION_REVISION                5
#define FWVERSION_BUILD                   1
#define LIB_INFO_FWVERSION                "3.0.5.1"


#define LIB_INFO_SOURCE_COMMIT_ID        "f9c1971fb2ba4f12eec8a596748c8ffa8f363811"
#define LIB_INFO_CONFIG_COMMIT_ID        "f9c1971fb2ba4f12eec8a596748c8ffa8f363811"
#define LIB_INFO_BUILD_TIME              "20201010165408"

#define LIB_INFO_BUILD_DATE_Y            2020
#define LIB_INFO_BUILD_DATE_M            10
#define LIB_INFO_BUILD_DATE_D            10

#define LIB_INFO_BUILD_TIME_H            16
#define LIB_INFO_BUILD_TIME_M            54
#define LIB_INFO_BUILD_TIME_S            8

#pragma push
#pragma CONST_SEG __GPAGE_SEG APP_INFO
extern const Boot_AppInfoTagType AppInfoTag;
extern boolean AppInfo_IsTestVersion(void);
#pragma pop
#endif
