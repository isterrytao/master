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
#define FWVERSION_REVISION                0
#define FWVERSION_BUILD                   0
#define LIB_INFO_FWVERSION                "4.0.0.0"


#define LIB_INFO_SOURCE_COMMIT_ID        "1dab8b31df9db3011d13a4609b872bc142d93664"
#define LIB_INFO_CONFIG_COMMIT_ID        "1dab8b31df9db3011d13a4609b872bc142d93664"
#define LIB_INFO_BUILD_TIME              "20211027200652"

#define LIB_INFO_BUILD_DATE_Y            2021
#define LIB_INFO_BUILD_DATE_M            10
#define LIB_INFO_BUILD_DATE_D            27

#define LIB_INFO_BUILD_TIME_H            20
#define LIB_INFO_BUILD_TIME_M            6
#define LIB_INFO_BUILD_TIME_S            52

#pragma push
#pragma CONST_SEG __GPAGE_SEG APP_INFO
extern const Boot_AppInfoTagType AppInfoTag;
extern boolean AppInfo_IsTestVersion(void);
#pragma pop
#endif
