#include "BootInfo.h"
#include "AppInfo.h"

#ifndef APP_INFO_SOURCE_COMMIT_ID
#define APP_INFO_SOURCE_COMMIT_ID ""
#warning "APP_INFO_SOURCE_COMMIT_ID is not defined"
#endif

#ifndef APP_INFO_CONFIG_COMMIT_ID
#define APP_INFO_CONFIG_COMMIT_ID ""
#warning "APP_INFO_CONFIG_COMMIT_ID is not defined"
#endif


#ifndef APP_INFO_BUILD_TIME
#define APP_INFO_BUILD_TIME {0}
#warning "APP_INFO_BUILD_TIME is not defined"
#endif

#ifdef __HC12__
void _Startup(void);
#else
#define _Startup 0x00
#endif

#ifdef APP_INFO_VERSION_ARRAY
const uint8 AppInfo_AppVerion[3] = APP_INFO_VERSION_ARRAY;
#endif

#pragma push
#pragma CONST_SEG __GPAGE_SEG APP_INFO
#pragma MESSAGE DISABLE C3401 // Resulting string is not zero terminated

/*<%- releaseFileName %>*/
const Boot_AppInfoTagType AppInfoTag = {
    /* ValidFlag */ {APP_INFO_FLAG_0, APP_INFO_FLAG_1},
    /* TagVersion */ APP_TAG_VERSION,
    /* FWID */ {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    /* BurnID */ {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    /* SourceCommitId */ APP_INFO_SOURCE_COMMIT_ID, /*lint !e784*/
    /* ConfigCommitId */ LIB_INFO_CONFIG_COMMIT_ID, /*lint !e784*/
    /* BuildTime */ APP_INFO_BUILD_TIME,
    /* FWName */ "U6/iU6",
    /* FWVersion */ LIB_INFO_FWVERSION,
    /* SupplierName */ "UDAN",
    /* CompatibleHW */ APP_INFO_COMPATIBLE_HW,
    /* Entry */ _Startup,
#if defined(FBL_XEP100)
    /* FWAddrSpace */ Boot_FWBootloadAddrSpace,
#else
    /* FWAddrSpace */ APP_INFO_FW_ADDR_SPACE,
#endif
    /*ReleaseName: <SW_NAME>_<PID>_<OID>_<T/R><RQ_VER> */ APP_INFO_RELEASE_NAME,
};
#pragma pop


boolean AppInfo_IsTestVersion(void) {
    return APP_INFO_TEST_VERSION;
}

#include <stdio.h>
#include "Shell.h"

const char shell_summary_appinfo[] = "Dump application Info";
const char shell_help_appinfo[] = "\
\n\
";

int shell_func_appinfo(int argc, const char *const *argv);
int shell_func_appinfo(int argc, const char *const *argv) {
    int ret;
    uint8 sid[41];
    if (argc == 1) {
        (void)memcpy(sid, AppInfoTag.SourceCommitId, 40U);
        sid[40] = 0U;
        (void)printf("TagVersion: %u\n", (uint16)AppInfoTag.TagVersion);
        (void)printf("FWName: %s\n", AppInfoTag.FWName);
        (void)printf("FWVersion: %s\n", AppInfoTag.FWVersion);
        (void)printf("CompatibleHW: 0x%08lX\n", AppInfoTag.CompatibleHW);
        (void)printf("SID: %s\n", sid);
        (void)printf("BuildTime: %s\n", AppInfoTag.BuildTime);
        (void)printf("ReleaseName: %s\n", AppInfoTag.ReleaseName);
        ret = 0;
    } else {
        ret = Shell_ArgsError(argv[0]);
    }
    return ret;
}
