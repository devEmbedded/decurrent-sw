// 14 channel digital capture using DCMI peripheral, compresses data and feeds to USB.
#pragma once

#include <stdint.h>

#define input_dcmi_BASEFREQ 80000000

void input_dcmi_init(void);

uint16_t input_dcmi_read(void);

void input_dcmi_config(int samplerate, uint16_t channel_mask);

void input_dcmi_start(void);
