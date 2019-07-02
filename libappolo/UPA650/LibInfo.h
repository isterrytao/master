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
#define FWVERSION_BUILD                   3
#define LIB_INFO_FWVERSION                "3.0.4.3"


#define LIB_INFO_SOURCE_COMMIT_ID        "2f5a89c852c1088b1629d94df7b079a34430f408"
#define LIB_INFO_CONFIG_COMMIT_ID        "2f5a89c852c1088b1629d94df7b079a34430f408"
#define LIB_INFO_BUILD_TIME              "20190702191055"

#define LIB_INFO_BUILD_DATE_Y            2019
#define LIB_INFO_BUILD_DATE_M            7
#define LIB_INFO_BUILD_DATE_D            2

#define LIB_INFO_BUILD_TIME_H            19
#define LIB_INFO_BUILD_TIME_M            10
#define LIB_INFO_BUILD_TIME_S            55

#pragma push
#pragma CONST_SEG __GPAGE_SEG APP_INFO
extern const Boot_AppInfoTagType AppInfoTag;
extern boolean AppInfo_IsTestVersion(void);
#pragma pop
#endif
