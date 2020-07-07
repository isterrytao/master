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
#define FWVERSION_BUILD                   0
#define LIB_INFO_FWVERSION                "3.0.5.0"


#define LIB_INFO_SOURCE_COMMIT_ID        "c02a9636458224fb0b77ebd36f002ba9d474c0c7"
#define LIB_INFO_CONFIG_COMMIT_ID        "c02a9636458224fb0b77ebd36f002ba9d474c0c7"
#define LIB_INFO_BUILD_TIME              "20200703134827"

#define LIB_INFO_BUILD_DATE_Y            2020
#define LIB_INFO_BUILD_DATE_M            7
#define LIB_INFO_BUILD_DATE_D            3

#define LIB_INFO_BUILD_TIME_H            13
#define LIB_INFO_BUILD_TIME_M            48
#define LIB_INFO_BUILD_TIME_S            27

#pragma push
#pragma CONST_SEG __GPAGE_SEG APP_INFO
extern const Boot_AppInfoTagType AppInfoTag;
extern boolean AppInfo_IsTestVersion(void);
#pragma pop
#endif
