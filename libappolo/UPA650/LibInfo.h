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


#define LIB_INFO_SOURCE_COMMIT_ID        "95f215a9fcfaee1f0a83bbaa5e4230c6d86ff263"
#define LIB_INFO_CONFIG_COMMIT_ID        "95f215a9fcfaee1f0a83bbaa5e4230c6d86ff263"
#define LIB_INFO_BUILD_TIME              "20190731114251"

#define LIB_INFO_BUILD_DATE_Y            2019
#define LIB_INFO_BUILD_DATE_M            7
#define LIB_INFO_BUILD_DATE_D            31

#define LIB_INFO_BUILD_TIME_H            11
#define LIB_INFO_BUILD_TIME_M            42
#define LIB_INFO_BUILD_TIME_S            51

#pragma push
#pragma CONST_SEG __GPAGE_SEG APP_INFO
extern const Boot_AppInfoTagType AppInfoTag;
extern boolean AppInfo_IsTestVersion(void);
#pragma pop
#endif
