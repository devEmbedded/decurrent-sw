#include "usb_hub.h"
#include <ch.h>
#include <hal.h>
#include <string.h>
#include "debug.h"
#include "board.h"

static const I2CConfig g_i2c4_cfg = {
    .timingr = STM32_TIMINGR_PRESC(14U)  | /* 120MHz/15 = 8MHz I2CCLK.           */
        STM32_TIMINGR_SCLDEL(0x08) | STM32_TIMINGR_SDADEL(0x04) |
        STM32_TIMINGR_SCLH(0x1e)   | STM32_TIMINGR_SCLL(0x26),
    .cr1 = 0,
    .cr2 = 0
};

static const uint8_t g_usb_hub_regs[0x22] = {
    0x00, 0x20,
    [2+0x00] = 0x24, 0x04, // Vendor ID
    [2+0x02] = 0x22, 0x24, // Product ID
    [2+0x04] = 0xA0, 0x00, // Device ID
    [2+0x06] = 0xAE, // CFG1: Bus powered, high speed disable, single TT, no over current sense
    [2+0x07] = 0x20, // CFG2
    [2+0x08] = 0x01, // CFG3: String support enabled
    [2+0x09] = 0x00, // Non-removable device register
    [2+0x0A] = 0x04, 0x04, // Port disable PDS / PDB: Port 2 disabled
    [2+0x0C] = 0x01, 0x01, // Max power MAXPS/MAXPB: Hub chip powered separately
    [2+0x0E] = 0x01, 0x01, // HUB ctrl max current HCMCS/HCMCB
    [2+0x10] = 0x32, // Power on time: 100 ms
    [2+0x11] = 0x04, 0x09, // Language ID = US English
    [2+0x13] = 11, // Manufacturer string len
    [2+0x14] = 23, // Product string len
    [2+0x15] = 8,  // Serial string len
};

static const uint8_t g_usb_hub_attach_cmd[0x3] = {
    0xFF, 0x01, 0x01
};

static bool write_data(const uint8_t *data, size_t len)
{
    return i2cMasterTransmitTimeout(&I2CD4, 0x2C, data, len, NULL, 0, TIME_MS2I(100)) == MSG_OK;
}

static bool write_string(uint8_t addr, const char *str)
{
    int len = strlen(str);
    uint8_t buf[34] = {0};
    buf[0] = addr;
    buf[1] = 32;
    for (int i = 0; i < 16 && i < len; i++)
    {
        buf[2 + i * 2] = str[i];
    }
    
    if (!write_data(buf, sizeof(buf)))
        return false;

    if (len > 16)
    {
        buf[0] = addr + 32;
        for (int i = 0; i < 16 && i + 16 < len; i++)
        {
            buf[2 + i * 2] = str[i + 16];
        }
        if (!write_data(buf, sizeof(buf)))
            return false;
    }

    return true;
}

void usb_hub_init(void)
{
    i2cStart(&I2CD4, &g_i2c4_cfg);
    palClearLine(LINE_HUB_RST);
    chThdSleepMilliseconds(10);
    palSetLine(LINE_HUB_RST);
    chThdSleepMilliseconds(100);

    bool status = true;

    status = status && write_data(g_usb_hub_regs, sizeof(g_usb_hub_regs));
    status = status && write_string(0x16, "devEmbedded");
    status = status && write_string(0x54, "DECurrent analysis port");
    status = status && write_string(0x92, "00000000");
    status = status && write_data(g_usb_hub_attach_cmd, sizeof(g_usb_hub_attach_cmd));

    if (!status)
    {
        i2cflags_t errors = i2cGetErrors(&I2CD4);
        dbg("USB Hub config failed: 0x%02x\n", errors);
        return;
    }
}