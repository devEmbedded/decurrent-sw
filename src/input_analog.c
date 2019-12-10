#include <ch.h>
#include <hal.h>
#include <assert.h>
#include <stm32h7xx.h>
#include "board.h"
#include "databuf.h"
#include "input_analog.h"



void input_analog_init(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_ADC12EN;
    RCC->AHB4ENR |= RCC_AHB4ENR_ADC3EN;
    RCC->APB4ENR |= RCC_APB4ENR_VREFEN;

    VREFBUF->CSR = VREFBUF_CSR_ENVR;
}
