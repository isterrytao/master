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


#define LIB_INFO_SOURCE_COMMIT_ID        "63174c2f1eea30888a40d36e1ddcf1047707e5dd"
#define LIB_INFO_CONFIG_COMMIT_ID        "63174c2f1eea30888a40d36e1ddcf1047707e5dd"
#define LIB_INFO_BUILD_TIME              "20200317154129"

#define LIB_INFO_BUILD_DATE_Y            2020
#define LIB_INFO_BUILD_DATE_M            3
#define LIB_INFO_BUILD_DATE_D            17

#define LIB_INFO_BUILD_TIME_H            15
#define LIB_INFO_BUILD_TIME_M            41
#define LIB_INFO_BUILD_TIME_S            29

#pragma push
#pragma CONST_SEG __GPAGE_SEG APP_INFO
extern const Boot_AppInfoTagType AppInfoTag;
extern boolean AppInfo_IsTestVersion(void);
#pragma pop
#endif
