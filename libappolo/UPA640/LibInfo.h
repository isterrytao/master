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
#define FWVERSION_BUILD                   7
#define LIB_INFO_FWVERSION                "3.0.3.17"


#define LIB_INFO_SOURCE_COMMIT_ID        "3cbb6558e50d06d4dc13697bdccd5f01febdf6ca"
#define LIB_INFO_CONFIG_COMMIT_ID        "3cbb6558e50d06d4dc13697bdccd5f01febdf6ca"
#define LIB_INFO_BUILD_TIME              "20190110134907"

#define LIB_INFO_BUILD_DATE_Y            2019
#define LIB_INFO_BUILD_DATE_M            1
#define LIB_INFO_BUILD_DATE_D            10

#define LIB_INFO_BUILD_TIME_H            13
#define LIB_INFO_BUILD_TIME_M            49
#define LIB_INFO_BUILD_TIME_S            7

#pragma push
#pragma CONST_SEG __GPAGE_SEG APP_INFO
extern const Boot_AppInfoTagType AppInfoTag;
extern boolean AppInfo_IsTestVersion(void);
#pragma pop
#endif
