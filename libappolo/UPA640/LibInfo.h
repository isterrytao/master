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
#define FWVERSION_BUILD                   8
#define LIB_INFO_FWVERSION                "3.0.3.18"


#define LIB_INFO_SOURCE_COMMIT_ID        "faff50403a645112632bed4b4c0fb5cc3002be60"
#define LIB_INFO_CONFIG_COMMIT_ID        "faff50403a645112632bed4b4c0fb5cc3002be60"
#define LIB_INFO_BUILD_TIME              "20190114170938"

#define LIB_INFO_BUILD_DATE_Y            2019
#define LIB_INFO_BUILD_DATE_M            1
#define LIB_INFO_BUILD_DATE_D            14

#define LIB_INFO_BUILD_TIME_H            17
#define LIB_INFO_BUILD_TIME_M            9
#define LIB_INFO_BUILD_TIME_S            38

#pragma push
#pragma CONST_SEG __GPAGE_SEG APP_INFO
extern const Boot_AppInfoTagType AppInfoTag;
extern boolean AppInfo_IsTestVersion(void);
#pragma pop
#endif
