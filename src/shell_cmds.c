#include <ch.h>
#include <hal.h>
#include <chprintf.h>
#include <stdlib.h>
#include <stm32h7xx.h>
#include "shell_cmds.h"
#include "databuf.h"
#include "output_vsw.h"
#include "input_digital.h"

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

static void cmd_output_vsw(BaseSequentialStream *chp, int argc, char *argv[])
{
    int voltage_mv = atoi(argv[0]);
    output_vsw_set(voltage_mv / 1000.0);
    chprintf(chp, "Vsw voltage set to %d mV\n", voltage_mv);
}

static void cmd_input_digital(BaseSequentialStream *chp, int argc, char *argv[])
{
    chprintf(chp, "Digital input: 0x%02x\n", input_digital_read());
}

const ShellCommand g_shell_cmds[] = {
    {"mem", cmd_mem},
    {"reboot", cmd_reboot},
    {"output_vsw", cmd_output_vsw},
    {"input_digital", cmd_input_digital},
    {NULL, NULL}
};
