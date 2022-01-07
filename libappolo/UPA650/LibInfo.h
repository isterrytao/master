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


#define LIB_INFO_SOURCE_COMMIT_ID        "0deea3008854e6b000dbe4e0d7159bde376e3373"
#define LIB_INFO_CONFIG_COMMIT_ID        "0deea3008854e6b000dbe4e0d7159bde376e3373"
#define LIB_INFO_BUILD_TIME              "20220107134026"

#define LIB_INFO_BUILD_DATE_Y            2022
#define LIB_INFO_BUILD_DATE_M            1
#define LIB_INFO_BUILD_DATE_D            7

#define LIB_INFO_BUILD_TIME_H            13
#define LIB_INFO_BUILD_TIME_M            40
#define LIB_INFO_BUILD_TIME_S            26

#pragma push
#pragma CONST_SEG __GPAGE_SEG APP_INFO
extern const Boot_AppInfoTagType AppInfoTag;
extern boolean AppInfo_IsTestVersion(void);
#pragma pop
#endif
