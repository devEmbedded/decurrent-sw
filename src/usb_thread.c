#include <ch.h>
#include <hal.h>
#include <shell.h>
#include <stdio.h>
#include "usb_thread.h"
#include "priorities.h"
#include "debug.h"
#include "shell_cmds.h"
#include "decurrent.pb.h"
#include "pb_encode.h"
#include "usbcfg.h"
#include "board.h"

static THD_WORKING_AREA(g_usb_shell_wa, 2048);
static thread_t *g_usb_shell_thread;

static msg_t g_usb_write_queue_mem[DATABUF_COUNT];
static mailbox_t g_usb_write_queue;

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
    while (!chThdShouldTerminateX())
    {
        databuf_t *buf = NULL;
        if (chMBFetchTimeout(&g_usb_write_queue, (msg_t*)&buf, TIME_MS2I(100)) == MSG_OK)
        {
            uint32_t len = buf->data[0];

            if (len % 512 != 0)
            {
                // Pad the message to 512 bytes. Sending partial USB packets can cause the host
                // to delay the next poll, causing lower throughput. We could also copy the beginning
                // of the next packet instead, but that would cause unaligned accesses. In most
                // cases it is easier to just insert a few dummy bytes.
                size_t padding = 512 - (len % 512);
                pb_ostream_t padstream = pb_ostream_from_buffer((uint8_t*)buf + len, DATABUF_BYTES - len);

                if (padding == 2)
                {
                    pb_encode_tag(&padstream, PB_WT_STRING, USBResponse_padding_tag);
                    pb_encode_varint(&padstream, 0);
                    len += padding;
                }
                else if (padding >= 3)
                {
                    pb_encode_tag(&padstream, PB_WT_STRING, USBResponse_padding_tag);
                    size_t bytes_len = padding - 3;
                    uint8_t varint[2] = {0x80 | (bytes_len & 0x7F), (bytes_len >> 7)};
                    pb_write(&padstream, varint, 2);
                    len += padding;
                }
            }

            msg_t msg = usbTransmit(&USBD2, USB_PROTOBUF_ENDPOINT, (uint8_t*)buf, len);
            databuf_release(&buf);

            if (msg != MSG_OK)
            {
                dbg("usbTransmit() failed: %d", (int)msg);
            }
        }
        else
        {
            uint8_t statusbuf[256];
            uint32_t total_len = 0;
            pb_ostream_t stream = pb_ostream_from_buffer(statusbuf, sizeof(statusbuf));
            pb_encode_fixed32(&stream, &total_len);

            USBResponse packet = USBResponse_init_zero;
            packet.status = Status_STATUS_IDLE;
            packet.cpu_usage = get_cpu_usage();
            
            if (!pb_encode(&stream, USBResponse_fields, &packet))
            {
                dbg("pb_encode failed: %s", stream.errmsg);
            }

            total_len = stream.bytes_written;
            stream = pb_ostream_from_buffer(statusbuf, sizeof(statusbuf));
            pb_encode_fixed32(&stream, &total_len);

            msg_t msg = usbTransmit(&USBD2, USB_PROTOBUF_ENDPOINT, statusbuf, total_len);
            
            if (msg != MSG_OK)
            {
                dbg("usbTransmit() failed: %d", (int)msg);
            }
        }
    }
}

void usb_thread_start()
{
    shellInit();
    chMBObjectInit(&g_usb_write_queue, g_usb_write_queue_mem, DATABUF_COUNT);
    chThdCreateStatic(g_usb_thread_wa, sizeof(g_usb_thread_wa), THDPRIO_USB, usb_thread, NULL);
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
                THDPRIO_SHELL, shellThread, (void*)&g_shell_cfg);
            chRegSetThreadNameX(g_usb_shell_thread, "USB Shell");
        }
    }

    if (g_usb_shell_thread && chThdTerminatedX(g_usb_shell_thread))
    {
        chThdRelease(g_usb_shell_thread);
        g_usb_shell_thread = NULL;
    }
}

void usb_thread_write(databuf_t *buf)
{
    chMBPostTimeout(&g_usb_write_queue, (msg_t)buf, TIME_INFINITE);
}
