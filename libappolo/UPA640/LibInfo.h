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
#define FWVERSION_BUILD                   5
#define LIB_INFO_FWVERSION                "3.0.3.25"


#define LIB_INFO_SOURCE_COMMIT_ID        "b3b9453ae56d6ee80072c6f8cad6d4fd43c38da9"
#define LIB_INFO_CONFIG_COMMIT_ID        "b3b9453ae56d6ee80072c6f8cad6d4fd43c38da9"
#define LIB_INFO_BUILD_TIME              "20190403133719"

#define LIB_INFO_BUILD_DATE_Y            2019
#define LIB_INFO_BUILD_DATE_M            4
#define LIB_INFO_BUILD_DATE_D            3

#define LIB_INFO_BUILD_TIME_H            13
#define LIB_INFO_BUILD_TIME_M            37
#define LIB_INFO_BUILD_TIME_S            19

#pragma push
#pragma CONST_SEG __GPAGE_SEG APP_INFO
extern const Boot_AppInfoTagType AppInfoTag;
extern boolean AppInfo_IsTestVersion(void);
#pragma pop
#endif
