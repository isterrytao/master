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


#define LIB_INFO_SOURCE_COMMIT_ID        "8f9ea968b93fd98b30b15126406e94475007b6ca"
#define LIB_INFO_CONFIG_COMMIT_ID        "8f9ea968b93fd98b30b15126406e94475007b6ca"
#define LIB_INFO_BUILD_TIME              "20190812191738"

#define LIB_INFO_BUILD_DATE_Y            2019
#define LIB_INFO_BUILD_DATE_M            8
#define LIB_INFO_BUILD_DATE_D            12

#define LIB_INFO_BUILD_TIME_H            19
#define LIB_INFO_BUILD_TIME_M            17
#define LIB_INFO_BUILD_TIME_S            38

#pragma push
#pragma CONST_SEG __GPAGE_SEG APP_INFO
extern const Boot_AppInfoTagType AppInfoTag;
extern boolean AppInfo_IsTestVersion(void);
#pragma pop
#endif
