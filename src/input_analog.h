// 3 channel analog inputs using ADC1, ADC2 and ADC3.
// Compresses data and feeds to USB.
#pragma once

#include <stdint.h>

#define INPUT_ANALOG_BASEFREQ 3000000

void input_analog_init(void);
