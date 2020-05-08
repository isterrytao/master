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


#define LIB_INFO_SOURCE_COMMIT_ID        "0ee0114661858a9c89cd8d06a84d2b4e0a7ff3e2"
#define LIB_INFO_CONFIG_COMMIT_ID        "0ee0114661858a9c89cd8d06a84d2b4e0a7ff3e2"
#define LIB_INFO_BUILD_TIME              "20200508111911"

#define LIB_INFO_BUILD_DATE_Y            2020
#define LIB_INFO_BUILD_DATE_M            5
#define LIB_INFO_BUILD_DATE_D            8

#define LIB_INFO_BUILD_TIME_H            11
#define LIB_INFO_BUILD_TIME_M            19
#define LIB_INFO_BUILD_TIME_S            11

#pragma push
#pragma CONST_SEG __GPAGE_SEG APP_INFO
extern const Boot_AppInfoTagType AppInfoTag;
extern boolean AppInfo_IsTestVersion(void);
#pragma pop
#endif
