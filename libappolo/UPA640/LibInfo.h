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


#define LIB_INFO_SOURCE_COMMIT_ID        "de06bc89766e9bdac81a5054b92f42f4fcf0adf3"
#define LIB_INFO_CONFIG_COMMIT_ID        "de06bc89766e9bdac81a5054b92f42f4fcf0adf3"
#define LIB_INFO_BUILD_TIME              "20200714112051"

#define LIB_INFO_BUILD_DATE_Y            2020
#define LIB_INFO_BUILD_DATE_M            7
#define LIB_INFO_BUILD_DATE_D            14

#define LIB_INFO_BUILD_TIME_H            11
#define LIB_INFO_BUILD_TIME_M            20
#define LIB_INFO_BUILD_TIME_S            51

#pragma push
#pragma CONST_SEG __GPAGE_SEG APP_INFO
extern const Boot_AppInfoTagType AppInfoTag;
extern boolean AppInfo_IsTestVersion(void);
#pragma pop
#endif
