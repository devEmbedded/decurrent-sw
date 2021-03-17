#include <ch.h>
#include <hal.h>
#include <assert.h>
#include <stm32h7xx.h>
#include "board.h"
#include "mdma.h"
#include "snappy32.h"
#include "decurrent.pb.h"
#include <pb_encode.h>
#include "priorities.h"
#include "usb_thread.h"
#include "databuf.h"
#include "debug.h"
#include "input_dcmi.h"

#define DCMI_PULLUP   (PAL_MODE_ALTERNATE(13) | PAL_STM32_PUPDR_PULLUP)
#define DCMI_PULLDOWN (PAL_MODE_ALTERNATE(13) | PAL_STM32_PUPDR_PULLDOWN)

void input_dcmi_init(void)
{
    RCC->AHB2ENR |= RCC_AHB2ENR_DCMIEN;
    RCC->AHB3ENR |= RCC_AHB3ENR_MDMAEN;
    RCC->APB1LENR |= RCC_APB1LENR_TIM2EN;
}

uint16_t input_dcmi_read(void)
{
    TIM2->CCMR1 = 0x0060;
    TIM2->ARR = 64;
    TIM2->CCR1 = 32;
    TIM2->CCER = 1;
    TIM2->CR1 = 1;

    palSetLineMode(LINE_DCMI_VSYNC, DCMI_PULLDOWN);
    chSysPolledDelayX(16);

    DCMI->CR = DCMI_CR_EDM_1 | DCMI_CR_EDM_0 | DCMI_CR_HSPOL | DCMI_CR_JPEG;
    DCMI->CR |= DCMI_CR_ENABLE;
    DCMI->CR |= DCMI_CR_CAPTURE;
    
    chSysPolledDelayX(16);
    palSetLineMode(LINE_DCMI_VSYNC, DCMI_PULLUP);

    while (!(DCMI->SR & DCMI_SR_FNE));

    uint16_t result = DCMI->DR;
    DCMI->CR = 0;
    return result;
}

void input_dcmi_config(int samplerate, uint16_t channel_mask)
{

}

void input_dcmi_start()
{

}
