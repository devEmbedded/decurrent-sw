#include <ch.h>
#include <hal.h>
#include "mdma.h"
#include "priorities.h"

static struct {
    mdma_callback_t callback;
    void *context;
} g_mdma_callbacks[16];

#define STM32_MDMA_HANDLER Vector228
OSAL_IRQ_HANDLER(STM32_MDMA_HANDLER) {
    OSAL_IRQ_PROLOGUE();

    uint32_t flags = MDMA->GISR0;
    while (flags)
    {
        int channel = 31 - __builtin_clz(flags);
        g_mdma_callbacks[channel].callback(channel, g_mdma_callbacks[channel].context);

        flags = MDMA->GISR0;
    }

    OSAL_IRQ_EPILOGUE();
}

void mdma_set_callback(int channel, mdma_callback_t callback, void *context)
{
    syssts_t state = chSysGetStatusAndLockX();
    g_mdma_callbacks[channel].callback = callback;
    g_mdma_callbacks[channel].context = context;
    chSysRestoreStatusX(state);
}

void mdma_init()
{
    RCC->AHB3ENR |= RCC_AHB3ENR_MDMAEN;
    NVIC_EnableIRQ(MDMA_IRQn);
    NVIC_SetPriority(MDMA_IRQn, IRQPRIO_MDMA);
}