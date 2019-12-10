#include <ch.h>
#include <hal.h>
#include <math.h>
#include <stm32h7xx.h>
#include <arm_math.h>
#include "databuf.h"
#include "output_analog.h"

#define DAC_BUFLEN 2048
#define VDD_VOLTS 3.3f

static DACConfig g_output_analog_config;

#define FXP_SCALE 0x10000
#define RND_MASK 0x3FF
#define DAC_SHIFT 12

__attribute__((section(".ram3")))
static uint16_t g_output_analog_buf[DAC_BUFLEN];
static struct {
    uint64_t samplenum;
    int dither;
    uint32_t rnd;
    q31_t angle;
    q31_t angle_increment;
} g_output_analog_state;

static int clamp(int x)
{
    if (x >= FXP_SCALE) x = FXP_SCALE - 1;
    if (x < 0) x = 0;
    return x;
}

static inline int apply_dither(int x, int *dither, uint32_t *rnd)
{
    int ix = (x + *dither + (*rnd & RND_MASK)) >> DAC_SHIFT;
    *dither += (x - ix << DAC_SHIFT);
    *rnd = ((uint64_t)*rnd * 4294967118UL) + *dither;
    return ix;
}

static void output_analog_fill_buffer_dc(uint16_t *buffer, size_t samples)
{
    int v = clamp(FXP_SCALE * g_output_analog_config.start.offset / VDD_VOLTS);
    int dither = g_output_analog_state.dither;
    uint32_t rnd = g_output_analog_state.rnd;
    for (int i = 0; i < samples; i++)
    {
        buffer[i] = apply_dither(v, &dither, &rnd);
    }
    g_output_analog_state.dither = dither;
    g_output_analog_state.rnd = rnd;
}

static void output_analog_fill_buffer_sin(uint16_t *buffer, size_t samples)
{
    
    int dither = g_output_analog_state.dither;
    uint32_t rnd = g_output_analog_state.rnd;
    for (int i = 0; i < samples; i++)
    {
        int v = clamp()
        buffer[i] = apply_dither(v, &dither, &rnd);
    }
}

static void output_analog_fill_buffer(uint16_t *buffer, size_t samples, uint64_t first_sample_idx)
{
        if (g_output_analog_config.wave == WAVE_DC)
        {
            q31_t val = roundf(g_output_analog_config.start.offset * 0x40000000);

    }
    q31_t 
    
    }
}

void output_analog_init(void)
{
    RCC->APB1LENR |= RCC_APB1LENR_DAC12EN;
}

void output_analog_config(const DACConfig *config)
{
    g_output_analog_config = *config;
    g_output_analog_config.start.amplitude /= 3.3f;
    g_output_analog_config.start.offset /= 3.3f;
    g_output_analog_config.end.amplitude /= 3.3f;
    g_output_analog_config.end.offset /= 3.3f;
}

void output_analog_start(void)
{
    
}

void output_analog_stop(void)
{
}