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
#define FWVERSION_REVISION                0
#define FWVERSION_BUILD                   0
#define LIB_INFO_FWVERSION                "4.0.0.0"


#define LIB_INFO_SOURCE_COMMIT_ID        "7b6f16403d474f4cbb19a0559b827c9fe4b7a2af"
#define LIB_INFO_CONFIG_COMMIT_ID        "7b6f16403d474f4cbb19a0559b827c9fe4b7a2af"
#define LIB_INFO_BUILD_TIME              "20210902165001"

#define LIB_INFO_BUILD_DATE_Y            2021
#define LIB_INFO_BUILD_DATE_M            9
#define LIB_INFO_BUILD_DATE_D            2

#define LIB_INFO_BUILD_TIME_H            16
#define LIB_INFO_BUILD_TIME_M            50
#define LIB_INFO_BUILD_TIME_S            1

#pragma push
#pragma CONST_SEG __GPAGE_SEG APP_INFO
extern const Boot_AppInfoTagType AppInfoTag;
extern boolean AppInfo_IsTestVersion(void);
#pragma pop
#endif
