#include "Shell.h"

const char *const shell_wellcome_message = "Hello, this is a shell.";
const char *const shell_prompt = "Shell> ";

SHELL_PROTYPE(appinfo);
SHELL_PROTYPE(cd4051);
SHELL_PROTYPE(hef4052);
SHELL_PROTYPE(ldmhv);
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
SHELL_PROTYPE(chtemp);
SHELL_PROTYPE(shunt);
SHELL_PROTYPE(cppwm);
SHELL_PROTYPE(cantest);
SHELL_PROTYPE(onboardt);
SHELL_PROTYPE(ltc);

const struct shell_command shell_commands[] = {
#ifdef __HC12__
    SHELL_COMMAND("io", io),
    SHELL_COMMAND("ai", analogi),
    SHELL_COMMAND("volmux", cd4051),
    SHELL_COMMAND("adc", adc),
    SHELL_COMMAND("port", port),
    SHELL_COMMAND("ldmgmux", hef4052),
    SHELL_COMMAND("ldmhv", ldmhv),
    SHELL_COMMAND("appinfo", appinfo),
    SHELL_COMMAND("nand", nand),
    SHELL_COMMAND("at24", at24),
    SHELL_COMMAND("flag", flag),
    SHELL_COMMAND("rtc", rtc),
    SHELL_COMMAND("reset", reset),
    SHELL_COMMAND("dtu", dtu),
    SHELL_COMMAND("chtemp", chtemp),
    SHELL_COMMAND("shunt", shunt),
    SHELL_COMMAND("cppwm", cppwm),
    SHELL_COMMAND("cantest", cantest),
    SHELL_COMMAND("onboardt", onboardt),
    SHELL_COMMAND("ltc", ltc),
#endif
    SHELL_COMMAND("dmem", dmem),
    SHELL_COMMAND("help", help),
    SHELL_COMMAND_END(),
};
