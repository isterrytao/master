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
#define FWVERSION_BUILD                   0
#define LIB_INFO_FWVERSION                "3.0.5.0"


#define LIB_INFO_SOURCE_COMMIT_ID        "b4442958617dbb64d1b0290caf8065914b7a891b"
#define LIB_INFO_CONFIG_COMMIT_ID        "b4442958617dbb64d1b0290caf8065914b7a891b"
#define LIB_INFO_BUILD_TIME              "20200522193703"

#define LIB_INFO_BUILD_DATE_Y            2020
#define LIB_INFO_BUILD_DATE_M            5
#define LIB_INFO_BUILD_DATE_D            22

#define LIB_INFO_BUILD_TIME_H            19
#define LIB_INFO_BUILD_TIME_M            37
#define LIB_INFO_BUILD_TIME_S            3

#pragma push
#pragma CONST_SEG __GPAGE_SEG APP_INFO
extern const Boot_AppInfoTagType AppInfoTag;
extern boolean AppInfo_IsTestVersion(void);
#pragma pop
#endif
