#include <ch.h>
#include <hal.h>
#include "board.h"

int main(void)
{
    halInit();
    chSysInit();

    while (true)
    {
        chThdSleepMilliseconds(500);
        palSetLine(LINE_LED_STATUS);
        chThdSleepMilliseconds(500);
        palClearLine(LINE_LED_STATUS);
    }
}