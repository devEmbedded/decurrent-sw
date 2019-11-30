#include <ch.h>
#include <hal.h>
#include <assert.h>
#include <stm32h7xx.h>
#include "board.h"
#include "databuf.h"
#include "input_digital.h"

#define MDMA_CHANNEL MDMA_Channel0

event_source_t g_input_digital_trigger_event;

static uint32_t g_digital_input_mask;
static uint32_t g_digital_input_divider;
static databuf_t *g_digital_input_current_buffer;
static databuf_t *g_digital_input_next_buffer;

// Swaps 0x76543210 to 0x46570213 to account for QuadSPI data order.
static uint32_t quadspi_preprocess(uint32_t x)
{
    uint32_t swap = ((x ^ (x >> 12)) & 0x000F000F) * 0x1001;
    return x ^ swap;
}

static uint32_t quadspi_preprocess_mask(uint32_t x)
{
    uint32_t swap = ((x ^ (x >> 12)) & 0x000F000F) * 0x1001;
    return (x ^ swap) & g_digital_input_mask;
}

uint8_t input_digital_read(void)
{
    RCC->AHB3ENR |= RCC_AHB3ENR_QSPIEN;
    QUADSPI->DCR = QUADSPI_DCR_FSIZE_Msk;
    QUADSPI->DLR = 3;
    QUADSPI->FCR = QUADSPI_FCR_CTCF;
    QUADSPI->CR = QUADSPI_CR_DFM | (15 << QUADSPI_CR_FTHRES_Pos) | QUADSPI_CR_EN;
    assert(!(QUADSPI->SR & QUADSPI_SR_BUSY));
    QUADSPI->CCR = (1 << QUADSPI_CCR_FMODE_Pos) | (3 << QUADSPI_CCR_DMODE_Pos) | (2 << QUADSPI_CCR_DCYC_Pos);
    while ((QUADSPI->SR & QUADSPI_SR_TCF) == 0);
    uint32_t result = quadspi_preprocess(QUADSPI->DR);
    QUADSPI->CR = 0;
    return result;
}

void input_digital_config(int samplerate, uint8_t channel_mask)
{
    int divider = INPUT_DIGITAL_BASEFREQ / samplerate;
    if (divider < 1) divider = 1;
    if (divider > 1) divider &= ~1;
    g_digital_input_divider = divider;
    g_digital_input_mask = channel_mask * 0x01010101UL;
}

void input_digital_start(void)
{
    g_digital_input_current_buffer = databuf_try_allocate();
    g_digital_input_next_buffer = databuf_try_allocate();
    assert(g_digital_input_current_buffer && g_digital_input_next_buffer);

    RCC->AHB3ENR |= RCC_AHB3ENR_QSPIEN | RCC_AHB3ENR_MDMAEN;
    QUADSPI->DCR = QUADSPI_DCR_FSIZE_Msk;
    QUADSPI->DLR = 0xFFFFFFFF;
    QUADSPI->CR = QUADSPI_CR_DFM | (15 << QUADSPI_CR_FTHRES_Pos) | QUADSPI_CR_EN;
    QUADSPI->CCR = (1 << QUADSPI_CCR_FMODE_Pos) | (3 << QUADSPI_CCR_DMODE_Pos) | (2 << QUADSPI_CCR_DCYC_Pos);
}