// 1 analog output channel using DAC1.
// DAC1 is 12-bit, this module performs noise shaping
// sigma delta modulation to gain more resolution.
#pragma once

#include <stdint.h>
#include "decurrent.pb.h"

void output_analog_init(void);
void output_analog_config(const DACConfig *config);
void output_analog_start(void);
void output_analog_stop(void);