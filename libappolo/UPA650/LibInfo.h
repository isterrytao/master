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


#define LIB_INFO_SOURCE_COMMIT_ID        "bbfeef6aad7ea7b2ac32c4b256f7b8858fcecf9a"
#define LIB_INFO_CONFIG_COMMIT_ID        "bbfeef6aad7ea7b2ac32c4b256f7b8858fcecf9a"
#define LIB_INFO_BUILD_TIME              "20190627210456"

#define LIB_INFO_BUILD_DATE_Y            2019
#define LIB_INFO_BUILD_DATE_M            6
#define LIB_INFO_BUILD_DATE_D            27

#define LIB_INFO_BUILD_TIME_H            21
#define LIB_INFO_BUILD_TIME_M            4
#define LIB_INFO_BUILD_TIME_S            56

#pragma push
#pragma CONST_SEG __GPAGE_SEG APP_INFO
extern const Boot_AppInfoTagType AppInfoTag;
extern boolean AppInfo_IsTestVersion(void);
#pragma pop
#endif
