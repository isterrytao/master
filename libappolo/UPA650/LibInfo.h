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
#define FWVERSION_BUILD                   0
#define LIB_INFO_FWVERSION                "3.0.4.0"


#define LIB_INFO_SOURCE_COMMIT_ID        "580002446d8b857889790afee9f9a0af87f42ac9"
#define LIB_INFO_CONFIG_COMMIT_ID        "580002446d8b857889790afee9f9a0af87f42ac9"
#define LIB_INFO_BUILD_TIME              "20190523170322"

#define LIB_INFO_BUILD_DATE_Y            2019
#define LIB_INFO_BUILD_DATE_M            5
#define LIB_INFO_BUILD_DATE_D            23

#define LIB_INFO_BUILD_TIME_H            17
#define LIB_INFO_BUILD_TIME_M            3
#define LIB_INFO_BUILD_TIME_S            22

#pragma push
#pragma CONST_SEG __GPAGE_SEG APP_INFO
extern const Boot_AppInfoTagType AppInfoTag;
extern boolean AppInfo_IsTestVersion(void);
#pragma pop
#endif
