// CIC^5 filtering for analog downsampling
#pragma once

#include <stdint.h>
#include <stdlib.h>

// 32-bit downsampler that has constant rate of 3x
typedef struct {
    uint32_t integrators[5];
    uint32_t combs[5][2];
    int phase;
} downsample32_state_t;

// 64-bit downsampler that has rate of 1..64x
typedef struct {
    uint64_t integrators[5];
    uint64_t combs[5][2];
    int phase;
    int divider;
    int shift;
    int gain;
} downsample64_state_t;

// Multi-stage downsampler that has rate of 1..1000000x
typedef struct {
    downsample64_state_t downsamplers[4];
    int stages;
    int divider;
    float gain;
} downsampleX_state_t;

#define DOWNSAMPLE32_GAIN (6*6*6*6*6)

void downsample32_init(downsample32_state_t *state);
size_t downsample32(downsample32_state_t *state, const uint16_t *input, size_t count, uint32_t *output);

void downsample64_init(downsample64_state_t *state, int divider, int shift);
size_t downsample64(downsample64_state_t *state, const uint32_t *input, size_t count, uint32_t *output);

void downsampleX_init(downsampleX_state_t *state, int divider, uint32_t input_range);
size_t downsampleX(downsampleX_state_t *state, const uint32_t *input, size_t count, uint32_t *output);

