// Configures the USB HUB chip using I2C.
// The purpose of the hub is to force communication on the analysis port
// to USB full-speed 12Mbit/s speed.
#pragma once

void usb_hub_init(void);