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


#define LIB_INFO_SOURCE_COMMIT_ID        "cb978fcb31458a5f7198102a0826261d3e84d487"
#define LIB_INFO_CONFIG_COMMIT_ID        "cb978fcb31458a5f7198102a0826261d3e84d487"
#define LIB_INFO_BUILD_TIME              "20201205232658"

#define LIB_INFO_BUILD_DATE_Y            2020
#define LIB_INFO_BUILD_DATE_M            12
#define LIB_INFO_BUILD_DATE_D            5

#define LIB_INFO_BUILD_TIME_H            23
#define LIB_INFO_BUILD_TIME_M            26
#define LIB_INFO_BUILD_TIME_S            58

#pragma push
#pragma CONST_SEG __GPAGE_SEG APP_INFO
extern const Boot_AppInfoTagType AppInfoTag;
extern boolean AppInfo_IsTestVersion(void);
#pragma pop
#endif
