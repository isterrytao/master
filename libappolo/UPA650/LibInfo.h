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


#define LIB_INFO_SOURCE_COMMIT_ID        "1a2fea9c118731f5cff43bc8d369b2f18c9e285a"
#define LIB_INFO_CONFIG_COMMIT_ID        "1a2fea9c118731f5cff43bc8d369b2f18c9e285a"
#define LIB_INFO_BUILD_TIME              "20210313164007"

#define LIB_INFO_BUILD_DATE_Y            2021
#define LIB_INFO_BUILD_DATE_M            3
#define LIB_INFO_BUILD_DATE_D            13

#define LIB_INFO_BUILD_TIME_H            16
#define LIB_INFO_BUILD_TIME_M            40
#define LIB_INFO_BUILD_TIME_S            7

#pragma push
#pragma CONST_SEG __GPAGE_SEG APP_INFO
extern const Boot_AppInfoTagType AppInfoTag;
extern boolean AppInfo_IsTestVersion(void);
#pragma pop
#endif
