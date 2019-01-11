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


#define LIB_INFO_SOURCE_COMMIT_ID        "ecff8be25cca0cc7d754838aa1ad1927763ac49f"
#define LIB_INFO_CONFIG_COMMIT_ID        "ecff8be25cca0cc7d754838aa1ad1927763ac49f"
#define LIB_INFO_BUILD_TIME              "20190111133240"

#define LIB_INFO_BUILD_DATE_Y            2019
#define LIB_INFO_BUILD_DATE_M            1
#define LIB_INFO_BUILD_DATE_D            11

#define LIB_INFO_BUILD_TIME_H            13
#define LIB_INFO_BUILD_TIME_M            32
#define LIB_INFO_BUILD_TIME_S            40

#pragma push
#pragma CONST_SEG __GPAGE_SEG APP_INFO
extern const Boot_AppInfoTagType AppInfoTag;
extern boolean AppInfo_IsTestVersion(void);
#pragma pop
#endif
