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


#define LIB_INFO_SOURCE_COMMIT_ID        "cae69aa2ca0ccb5733514ac94e8cf1b9fdf1ae5d"
#define LIB_INFO_CONFIG_COMMIT_ID        "cae69aa2ca0ccb5733514ac94e8cf1b9fdf1ae5d"
#define LIB_INFO_BUILD_TIME              "20220331130625"

#define LIB_INFO_BUILD_DATE_Y            2022
#define LIB_INFO_BUILD_DATE_M            3
#define LIB_INFO_BUILD_DATE_D            31

#define LIB_INFO_BUILD_TIME_H            13
#define LIB_INFO_BUILD_TIME_M            6
#define LIB_INFO_BUILD_TIME_S            25

#pragma push
#pragma CONST_SEG __GPAGE_SEG APP_INFO
extern const Boot_AppInfoTagType AppInfoTag;
extern boolean AppInfo_IsTestVersion(void);
#pragma pop
#endif
