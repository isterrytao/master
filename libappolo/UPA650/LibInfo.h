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


#define LIB_INFO_SOURCE_COMMIT_ID        "ddaf26f62b1244856fb89a1ce67d18b85e3d9e31"
#define LIB_INFO_CONFIG_COMMIT_ID        "ddaf26f62b1244856fb89a1ce67d18b85e3d9e31"
#define LIB_INFO_BUILD_TIME              "20190404101700"

#define LIB_INFO_BUILD_DATE_Y            2019
#define LIB_INFO_BUILD_DATE_M            4
#define LIB_INFO_BUILD_DATE_D            4

#define LIB_INFO_BUILD_TIME_H            10
#define LIB_INFO_BUILD_TIME_M            17
#define LIB_INFO_BUILD_TIME_S            0

#pragma push
#pragma CONST_SEG __GPAGE_SEG APP_INFO
extern const Boot_AppInfoTagType AppInfoTag;
extern boolean AppInfo_IsTestVersion(void);
#pragma pop
#endif
