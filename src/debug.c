#include <ch.h>
#include <hal.h>
#include <stm32h7xx.h>
#include <chprintf.h>
#include <memstreams.h>
#include <string.h>
#include "board.h"

static char g_dbgmsg[128];

void vdbg(const char *fmt, va_list ap)
{
  MemoryStream ms;
  msObjectInit(&ms, (uint8_t *)g_dbgmsg, sizeof(g_dbgmsg) - 1, 0);
  memset(g_dbgmsg, 0, sizeof(g_dbgmsg));
  
  chvprintf((BaseSequentialStream*)&ms, fmt, ap);
  chvprintf((BaseSequentialStream*)&ms, "\r\n", ap);
  
  while(1);
}

void dbg(const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  vdbg(fmt, ap);
  va_end(ap);
}

void abort_with_error(const char *fmt, ...)
{
  __disable_irq();
  
  for (;;)
  {
    va_list ap;
    va_start(ap, fmt);
    vdbg(fmt, ap);
    va_end(ap);
    chSysPolledDelayX(480000000);
  }
}

uint32_t *HARDFAULT_PSP;

void __attribute__((naked)) HardFault_Handler(void)
{
    asm("mrs %0, psp" : "=r"(HARDFAULT_PSP) : :);
    abort_with_error("HF %08x %08x", HARDFAULT_PSP[6], SCB->CFSR);
}
