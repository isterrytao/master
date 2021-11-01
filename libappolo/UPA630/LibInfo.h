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


#define LIB_INFO_SOURCE_COMMIT_ID        "2fe6a870afd1432f650fe2a4110c1dd35819baa3"
#define LIB_INFO_CONFIG_COMMIT_ID        "2fe6a870afd1432f650fe2a4110c1dd35819baa3"
#define LIB_INFO_BUILD_TIME              "20211101113416"

#define LIB_INFO_BUILD_DATE_Y            2021
#define LIB_INFO_BUILD_DATE_M            11
#define LIB_INFO_BUILD_DATE_D            1

#define LIB_INFO_BUILD_TIME_H            11
#define LIB_INFO_BUILD_TIME_M            34
#define LIB_INFO_BUILD_TIME_S            16

#pragma push
#pragma CONST_SEG __GPAGE_SEG APP_INFO
extern const Boot_AppInfoTagType AppInfoTag;
extern boolean AppInfo_IsTestVersion(void);
#pragma pop
#endif
