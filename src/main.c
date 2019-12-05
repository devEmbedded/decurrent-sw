#include <ch.h>
#include <hal.h>
#include "board.h"
#include "usb_thread.h"
#include "usb_hub.h"
#include "databuf.h"
#include "input_analog.h"
#include "input_digital.h"

int main(void)
{
    halInit();
    chSysInit();

    databuf_init();
    input_analog_init();
    input_digital_init();

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