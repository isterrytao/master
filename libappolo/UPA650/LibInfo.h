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
#define FWVERSION_BUILD                   2
#define LIB_INFO_FWVERSION                "3.0.4.2"


#define LIB_INFO_SOURCE_COMMIT_ID        "1a05ae58729c4bcf3b57931b5d298a53d9ac7638"
#define LIB_INFO_CONFIG_COMMIT_ID        "1a05ae58729c4bcf3b57931b5d298a53d9ac7638"
#define LIB_INFO_BUILD_TIME              "20190614180702"

#define LIB_INFO_BUILD_DATE_Y            2019
#define LIB_INFO_BUILD_DATE_M            6
#define LIB_INFO_BUILD_DATE_D            14

#define LIB_INFO_BUILD_TIME_H            18
#define LIB_INFO_BUILD_TIME_M            7
#define LIB_INFO_BUILD_TIME_S            2

#pragma push
#pragma CONST_SEG __GPAGE_SEG APP_INFO
extern const Boot_AppInfoTagType AppInfoTag;
extern boolean AppInfo_IsTestVersion(void);
#pragma pop
#endif
