#include <ch.h>
#include <hal.h>
#include <chprintf.h>
#include <stdlib.h>
#include <stm32h7xx.h>
#include "shell_cmds.h"
#include "databuf.h"

static void cmd_mem(BaseSequentialStream *chp, int argc, char *argv[]) {
  size_t n, total, largest;
  n = chHeapStatus(NULL, &total, &largest);
  chprintf(chp, "core free memory : %u bytes\n", chCoreGetStatusX());
  chprintf(chp, "heap fragments   : %u\n", n);
  chprintf(chp, "heap free total  : %u bytes\n", total);
  chprintf(chp, "heap free largest: %u bytes\n", largest);
  chprintf(chp, "free data buffers: %d blocks\n", databuf_get_free_count());
}

static void cmd_reboot(BaseSequentialStream *chp, int argc, char *argv[])
{
    NVIC_SystemReset();
}

const ShellCommand g_shell_cmds[] = {
    {"mem", cmd_mem},
    {"reboot", cmd_reboot},
    {NULL, NULL}
};
