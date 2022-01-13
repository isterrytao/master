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
#define FWVERSION_REVISION                1
#define FWVERSION_BUILD                   0
#define LIB_INFO_FWVERSION                "4.0.1.0"


#define LIB_INFO_SOURCE_COMMIT_ID        "7edfa1f23c20149c8a3b0143e88dbec29bcc1acb"
#define LIB_INFO_CONFIG_COMMIT_ID        "7edfa1f23c20149c8a3b0143e88dbec29bcc1acb"
#define LIB_INFO_BUILD_TIME              "20220111180811"

#define LIB_INFO_BUILD_DATE_Y            2022
#define LIB_INFO_BUILD_DATE_M            1
#define LIB_INFO_BUILD_DATE_D            11

#define LIB_INFO_BUILD_TIME_H            18
#define LIB_INFO_BUILD_TIME_M            8
#define LIB_INFO_BUILD_TIME_S            11

#pragma push
#pragma CONST_SEG __GPAGE_SEG APP_INFO
extern const Boot_AppInfoTagType AppInfoTag;
extern boolean AppInfo_IsTestVersion(void);
#pragma pop
#endif
