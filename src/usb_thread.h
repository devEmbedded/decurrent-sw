// Thread to handle USB HS communication with control PC
#pragma once
#include "databuf.h"

void usb_thread_start(void);
void usb_thread_poll(void);

// Add a buffer into write queue for the BULK IN endpoint.
// Each buffer should begin with a 32-bit length prefix.
void usb_thread_write(databuf_t *buf);