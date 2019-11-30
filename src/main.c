#include <ch.h>
#include <hal.h>
#include "board.h"
#include "usb_thread.h"
#include "databuf.h"

int main(void)
{
    halInit();
    chSysInit();

    databuf_init();
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