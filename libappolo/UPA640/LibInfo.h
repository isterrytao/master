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


#define LIB_INFO_SOURCE_COMMIT_ID        "1cc88bab96b7f771568b3b0e91cb82adf5a58fa5"
#define LIB_INFO_CONFIG_COMMIT_ID        "1cc88bab96b7f771568b3b0e91cb82adf5a58fa5"
#define LIB_INFO_BUILD_TIME              "20200508105340"

#define LIB_INFO_BUILD_DATE_Y            2020
#define LIB_INFO_BUILD_DATE_M            5
#define LIB_INFO_BUILD_DATE_D            8

#define LIB_INFO_BUILD_TIME_H            10
#define LIB_INFO_BUILD_TIME_M            53
#define LIB_INFO_BUILD_TIME_S            40

#pragma push
#pragma CONST_SEG __GPAGE_SEG APP_INFO
extern const Boot_AppInfoTagType AppInfoTag;
extern boolean AppInfo_IsTestVersion(void);
#pragma pop
#endif
