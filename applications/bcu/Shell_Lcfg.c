#include "Shell.h"

const char *const shell_wellcome_message = "Hello, this is a shell.";
const char *const shell_prompt = "Shell> ";

SHELL_PROTYPE(appinfo);
#if defined(A640) || defined(A641)

#else
SHELL_PROTYPE(cd4051);
#endif
#if defined(UPC6000)
SHELL_PROTYPE(hef4052);
#endif

#if defined(A650) || defined(A651)
SHELL_PROTYPE(ldmhv_A650);
#elif defined(A652) || defined(A653)
SHELL_PROTYPE(ldmhv_A652);
#elif defined(A640) || defined(A641)
#else
SHELL_PROTYPE(ldmhv);
#endif

SHELL_PROTYPE(nand);
SHELL_PROTYPE(at24);
SHELL_PROTYPE(rtc);
SHELL_PROTYPE(port);
SHELL_PROTYPE(io);
SHELL_PROTYPE(adc);
SHELL_PROTYPE(analogi);
SHELL_PROTYPE(flag);
SHELL_PROTYPE(reset);
SHELL_PROTYPE(dtu);
#if defined(UPC6000)
SHELL_PROTYPE(chtemp);
#endif
#if defined(UPA650)
SHELL_PROTYPE(chgtemp);
#endif
#if defined(UPA640)
SHELL_PROTYPE(chgtemp);
#endif

#if defined(A640) || defined(A641)
#else
SHELL_PROTYPE(shunt);
#endif
SHELL_PROTYPE(cppwm);
SHELL_PROTYPE(cantest);
SHELL_PROTYPE(onboardt);
#if defined(A600) || defined(A601) || defined(A602) || defined(A603)
SHELL_PROTYPE(ltc);
#endif
#if defined(A650) || defined(A651) || defined(A652) || defined(A653)
SHELL_PROTYPE(isl);
SHELL_PROTYPE(rs485);
#endif
#if defined(A640) || defined(A641)
SHELL_PROTYPE(isl);
#endif
SHELL_PROTYPE(dtustart);
SHELL_PROTYPE(gb34014);

const struct shell_command shell_commands[] = {
#ifdef __HC12__
    SHELL_COMMAND("io", io),
    SHELL_COMMAND("ai", analogi),
#if defined(A640) || defined(A641)
#else
    SHELL_COMMAND("volmux", cd4051),
#endif
    SHELL_COMMAND("adc", adc),
    SHELL_COMMAND("port", port),
#if defined(UPC6000)
    SHELL_COMMAND("ldmgmux", hef4052),
#endif

#if defined(A650) || defined(A651)
    SHELL_COMMAND("ldmhv", ldmhv_A650),
#elif defined(A652) || defined(A653)
    SHELL_COMMAND("ldmhv", ldmhv_A652),
#elif defined(A640) || defined(A641)
#else
    SHELL_COMMAND("ldmhv", ldmhv),
#endif
    SHELL_COMMAND("appinfo", appinfo),
    SHELL_COMMAND("nand", nand),
    SHELL_COMMAND("at24", at24),
    SHELL_COMMAND("flag", flag),
    SHELL_COMMAND("rtc", rtc),
    SHELL_COMMAND("reset", reset),
    SHELL_COMMAND("dtu", dtu),
#if defined(UPC6000)
    SHELL_COMMAND("chtemp", chtemp),
#endif
#if defined(UPA650)
    SHELL_COMMAND("chgtemp", chgtemp),
#endif
#if defined(UPA640)
        SHELL_COMMAND("chgtemp", chgtemp),
#endif
#if defined(A640) || defined(A641)
#else
    SHELL_COMMAND("shunt", shunt),
#endif
    SHELL_COMMAND("cppwm", cppwm),
    SHELL_COMMAND("cantest", cantest),
    SHELL_COMMAND("onboardt", onboardt),
#if defined(A600) || defined(A601) || defined(A602) || defined(A603)
    SHELL_COMMAND("ltc", ltc),
#endif
#if defined(A650) || defined(A651) || defined(A652) || defined(A653)
    SHELL_COMMAND("ltc", isl),
    SHELL_COMMAND("rs485", rs485),
#endif
#if defined(A640) || defined(A641)
    SHELL_COMMAND("ltc", isl),
#endif

#endif
    SHELL_COMMAND("dmem", dmem),
    SHELL_COMMAND("dtustart", dtustart),
    SHELL_COMMAND("gb34014", gb34014),
    SHELL_COMMAND("help", help),
    SHELL_COMMAND_END(),
};
