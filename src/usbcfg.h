// USB descriptors for the USB high-speed interface to control PC

#pragma once
#include <hal.h>

#define USB_SERIAL_DATA_ENDPOINT 1
#define USB_SERIAL_IRQ_ENDPOINT 2
#define USB_PROTOBUF_ENDPOINT 3

extern const USBConfig g_usbcfg;

extern SerialUSBDriver g_usb_serial;
extern const SerialUSBConfig g_usb_serial_config;
