#include <ch.h>
#include <hal.h>
#include <math.h>
#include <stm32h7xx.h>

#define PWM_PERIOD 1024

void output_vsw_set(float volts)
{
    int duty = roundf(volts * PWM_PERIOD / 3.3f);
    if (duty < 0) duty = 0;
    if (duty > PWM_PERIOD) duty = PWM_PERIOD;

    RCC->APB2ENR |= RCC_APB2ENR_TIM15EN;
    TIM15->AF1 = 0;
    TIM15->PSC = 0;
    TIM15->ARR = PWM_PERIOD - 1;
    TIM15->CCR1 = duty;
    TIM15->CCMR1 = 0x64;
    TIM15->CCER |= TIM_CCER_CC1E;
    TIM15->CR1 = TIM_CR1_CEN;
    TIM15->BDTR |= TIM_BDTR_MOE;
}
