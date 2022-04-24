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
#define FWVERSION_REVISION                2
#define FWVERSION_BUILD                   0
#define LIB_INFO_FWVERSION                "4.0.2.0"


#define LIB_INFO_SOURCE_COMMIT_ID        "11ae9eae876fb5710eda280a76589bd5f626d36b"
#define LIB_INFO_CONFIG_COMMIT_ID        "11ae9eae876fb5710eda280a76589bd5f626d36b"
#define LIB_INFO_BUILD_TIME              "20220419153312"

#define LIB_INFO_BUILD_DATE_Y            2022
#define LIB_INFO_BUILD_DATE_M            4
#define LIB_INFO_BUILD_DATE_D            19

#define LIB_INFO_BUILD_TIME_H            15
#define LIB_INFO_BUILD_TIME_M            33
#define LIB_INFO_BUILD_TIME_S            12

#pragma push
#pragma CONST_SEG __GPAGE_SEG APP_INFO
extern const Boot_AppInfoTagType AppInfoTag;
extern boolean AppInfo_IsTestVersion(void);
#pragma pop
#endif
