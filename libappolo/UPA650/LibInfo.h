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
#define FWVERSION_BUILD                   4
#define LIB_INFO_FWVERSION                "3.0.4.4"


#define LIB_INFO_SOURCE_COMMIT_ID        "20b2e5e3fb3d4c909fd8d21bd702df3589c6860a"
#define LIB_INFO_CONFIG_COMMIT_ID        "20b2e5e3fb3d4c909fd8d21bd702df3589c6860a"
#define LIB_INFO_BUILD_TIME              "20191121203237"

#define LIB_INFO_BUILD_DATE_Y            2019
#define LIB_INFO_BUILD_DATE_M            11
#define LIB_INFO_BUILD_DATE_D            21

#define LIB_INFO_BUILD_TIME_H            20
#define LIB_INFO_BUILD_TIME_M            32
#define LIB_INFO_BUILD_TIME_S            37

#pragma push
#pragma CONST_SEG __GPAGE_SEG APP_INFO
extern const Boot_AppInfoTagType AppInfoTag;
extern boolean AppInfo_IsTestVersion(void);
#pragma pop
#endif
