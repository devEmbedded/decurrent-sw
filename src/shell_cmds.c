#include <ch.h>
#include <hal.h>
#include <chprintf.h>
#include "shell_cmds.h"

static void cmd_reboot(BaseSequentialStream *chp, int argc, char *argv[])
{
    NVIC_SystemReset();
}

const ShellCommand g_shell_cmds[] = {
    {"reboot", cmd_reboot},
    {NULL, NULL}
};
