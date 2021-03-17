#include <ch.h>
#include <hal.h>
#include "board.h"
#include "usb_thread.h"
#include "usb_hub.h"
#include "mdma.h"
#include "databuf.h"
#include "input_analog.h"
#include "input_quadspi.h"
#include "input_dcmi.h"
#include "input_spi.h"
#include "output_vsw.h"

int main(void)
{
    halInit();
    chSysInit();

    mdma_init();
    databuf_init();
    // input_analog_init();
    input_quadspi_init();
    input_dcmi_init();
    input_spi_init();

    output_vsw_set(3.3f);
    usb_hub_init();
    usb_thread_start();

    while (true)
    {
        chThdSleepMilliseconds(500);
        palSetLine(LINE_LED_STATUS);
        chThdSleepMilliseconds(500);
        palClearLine(LINE_LED_STATUS);

        usb_thread_poll();
    }
}