#include <ch.h>
#include <hal.h>
#include <shell.h>
#include <stdio.h>
#include "usb_thread.h"
#include "shell_cmds.h"
#include "usbcfg.h"
#include "board.h"

static THD_WORKING_AREA(g_usb_shell_wa, 2048);
static thread_t *g_usb_shell_thread;

static THD_WORKING_AREA(g_usb_thread_wa, 2048);
static THD_FUNCTION(usb_thread, arg) {
    chRegSetThreadName("USB");

    usbDisconnectBus(&USBD2);
    palSetLine(LINE_USB_PHY_RST);
    chThdSleepMilliseconds(250);
    palClearLine(LINE_USB_PHY_RST);
    chThdSleepMilliseconds(10);
    
    sduObjectInit(&g_usb_serial);
    sduStart(&g_usb_serial, &g_usb_serial_config);
    
    usbStart(&USBD2, &g_usbcfg);
    usbConnectBus(&USBD2);

    int i = 0;
    while (true)
    {
        char buf[16];
        snprintf(buf, sizeof(buf), "Line %d\n", i++);
        msg_t msg = usbTransmit(&USBD2, USB_PROTOBUF_ENDPOINT, (uint8_t*)buf, sizeof(buf));
        
        if (msg != MSG_OK)
        {
            chThdSleepMilliseconds(500);
        }
    }
}

void usb_thread_start()
{
    shellInit();
    chThdCreateStatic(g_usb_thread_wa, sizeof(g_usb_thread_wa), NORMALPRIO, usb_thread, NULL);
}

static const ShellConfig g_shell_cfg = {
  (BaseSequentialStream *)&g_usb_serial,
  g_shell_cmds
};

void usb_thread_poll()
{
    if (USBD2.state == USB_ACTIVE)
    {
        if (g_usb_shell_thread == NULL)
        {
            g_usb_shell_thread = chThdCreateStatic(g_usb_shell_wa, sizeof(g_usb_shell_wa),
                NORMALPRIO - 1, shellThread, (void*)&g_shell_cfg);
            chRegSetThreadNameX(g_usb_shell_thread, "USB Shell")
        }
    }

    if (g_usb_shell_thread && chThdTerminatedX(g_usb_shell_thread))
    {
        chThdRelease(g_usb_shell_thread);
        g_usb_shell_thread = NULL;
    }
}