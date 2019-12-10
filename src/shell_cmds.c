#include <ch.h>
#include <hal.h>
#include <chprintf.h>
#include <stdlib.h>
#include <stm32h7xx.h>
#include "shell_cmds.h"
#include "debug.h"
#include "databuf.h"
#include "output_vsw.h"
#include "input_digital.h"

static void cmd_status(BaseSequentialStream *chp, int argc, char *argv[]) {
  size_t n, total, largest;
  n = chHeapStatus(NULL, &total, &largest);
  chprintf(chp, "core free memory : %u bytes\n", chCoreGetStatusX());
  chprintf(chp, "heap fragments   : %u\n", n);
  chprintf(chp, "heap free total  : %u bytes\n", total);
  chprintf(chp, "heap free largest: %u bytes\n", largest);
  chprintf(chp, "free data buffers: %d blocks\n", databuf_get_free_count());
  chprintf(chp, "cpu usage:         %d %%\n", get_cpu_usage());
}

static void cmd_reboot(BaseSequentialStream *chp, int argc, char *argv[])
{
    NVIC_SystemReset();
}

static void cmd_dmesg(BaseSequentialStream *chp, int argc, char *argv[])
{
    chprintf(chp, "\n");
    debug_print_log(chp);
    chprintf(chp, "\n");
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

    if (argc == 2)
    {
        int samplerate = strtoul(argv[0], NULL, 0);
        int mask = strtoul(argv[1], NULL, 0);
        chprintf(chp, "Starting capture at %d Hz with mask 0x%02x\n", samplerate, mask);
        input_digital_config(samplerate, mask);
        input_digital_start();
    }
}

const ShellCommand g_shell_cmds[] = {
    {"status", cmd_status},
    {"reboot", cmd_reboot},
    {"dmesg", cmd_dmesg},
    {"output_vsw", cmd_output_vsw},
    {"input_digital", cmd_input_digital},
    {NULL, NULL}
};
