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


#define LIB_INFO_SOURCE_COMMIT_ID        "dcd224b3778bbdc61c1c2fa2e482fa9afe78a910"
#define LIB_INFO_CONFIG_COMMIT_ID        "dcd224b3778bbdc61c1c2fa2e482fa9afe78a910"
#define LIB_INFO_BUILD_TIME              "20190403123055"

#define LIB_INFO_BUILD_DATE_Y            2019
#define LIB_INFO_BUILD_DATE_M            4
#define LIB_INFO_BUILD_DATE_D            3

#define LIB_INFO_BUILD_TIME_H            12
#define LIB_INFO_BUILD_TIME_M            30
#define LIB_INFO_BUILD_TIME_S            55

#pragma push
#pragma CONST_SEG __GPAGE_SEG APP_INFO
extern const Boot_AppInfoTagType AppInfoTag;
extern boolean AppInfo_IsTestVersion(void);
#pragma pop
#endif
