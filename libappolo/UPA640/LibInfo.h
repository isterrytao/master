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
#define FWVERSION_REVISION                4
#define FWVERSION_BUILD                   1
#define LIB_INFO_FWVERSION                "3.0.4.1"


#define LIB_INFO_SOURCE_COMMIT_ID        "b9522a599fb831272f68b91af19c972155db9e0d"
#define LIB_INFO_CONFIG_COMMIT_ID        "b9522a599fb831272f68b91af19c972155db9e0d"
#define LIB_INFO_BUILD_TIME              "20190527131800"

#define LIB_INFO_BUILD_DATE_Y            2019
#define LIB_INFO_BUILD_DATE_M            5
#define LIB_INFO_BUILD_DATE_D            27

#define LIB_INFO_BUILD_TIME_H            13
#define LIB_INFO_BUILD_TIME_M            18
#define LIB_INFO_BUILD_TIME_S            0

#pragma push
#pragma CONST_SEG __GPAGE_SEG APP_INFO
extern const Boot_AppInfoTagType AppInfoTag;
extern boolean AppInfo_IsTestVersion(void);
#pragma pop
#endif
