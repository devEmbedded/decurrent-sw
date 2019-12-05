// 8 channel digital capture using QuadSPI peripheral, compresses data and feeds to USB.
#pragma once

#include <stdint.h>

#define INPUT_DIGITAL_BASEFREQ 200000000

struct event_source_t;
extern event_source_t g_input_digital_trigger_event;

void input_digital_init(void);

// Read current state of digital inputs
uint8_t input_digital_read(void);

// Configure digital input capture
// samplerate must be an even fraction of INPUT_DIGITAL_BASEFREQ (i.e. /1, /2, /4, /6, /8, ...)
// channel_mask is 0xFF to capture all channels
void input_digital_config(int samplerate, uint8_t channel_mask);

// Start capturing immediately
void input_digital_start(void);

// Wait for trigger, start capturing and posts g_input_digital_trigger_event when triggered
// trigger_low has '1' in the channels that must have low level to trigger capture
// trigger_high has '1' in the channels that must have high level to trigger capture
// pretrigger_samples is the number of samples to keep before trigger event
void input_digital_arm_trigger(uint8_t trigger_low, uint8_t trigger_high, int pretrigger_samples);

// Get number of samples captured so far
uint64_t input_digital_get_count(void);

// Stop capturing immediately
void input_digital_stop(void);