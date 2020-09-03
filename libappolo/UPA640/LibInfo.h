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


#define LIB_INFO_SOURCE_COMMIT_ID        "45b91bd6ab3c4a801de7a744983bf254ada1a5e8"
#define LIB_INFO_CONFIG_COMMIT_ID        "45b91bd6ab3c4a801de7a744983bf254ada1a5e8"
#define LIB_INFO_BUILD_TIME              "20200903101024"

#define LIB_INFO_BUILD_DATE_Y            2020
#define LIB_INFO_BUILD_DATE_M            9
#define LIB_INFO_BUILD_DATE_D            3

#define LIB_INFO_BUILD_TIME_H            10
#define LIB_INFO_BUILD_TIME_M            10
#define LIB_INFO_BUILD_TIME_S            24

#pragma push
#pragma CONST_SEG __GPAGE_SEG APP_INFO
extern const Boot_AppInfoTagType AppInfoTag;
extern boolean AppInfo_IsTestVersion(void);
#pragma pop
#endif
