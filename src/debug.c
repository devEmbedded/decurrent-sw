#include <ch.h>
#include <hal.h>
#include <stm32h7xx.h>
#include <chprintf.h>
#include <string.h>
#include "board.h"

#define LOG_SIZE 4096
#define LOG_MASK 4095

static uint8_t g_logbuffer[LOG_SIZE];
static uint32_t g_logbuffer_wrpos;

static size_t log_write(void *instance, const uint8_t *bp, size_t n)
{
  for (size_t i = 0; i < n; i++)
  {
    g_logbuffer[g_logbuffer_wrpos & LOG_MASK] = bp[i];
    g_logbuffer_wrpos++;
  }
  return n;
}

static msg_t log_put(void *instance, uint8_t b)
{
  g_logbuffer[g_logbuffer_wrpos & LOG_MASK] = b;
  g_logbuffer_wrpos++;
  return MSG_OK;
}

static struct BaseSequentialStreamVMT g_logstream_methods = {
  0, &log_write, NULL, &log_put, NULL
};

static BaseSequentialStream g_logstream = {
  &g_logstream_methods
};

void debug_print_log(BaseSequentialStream *chp)
{
  uint32_t pos = g_logbuffer_wrpos;
  if (pos == 0)
  {
    return;
  }
  else if (pos <= LOG_SIZE || (pos & LOG_MASK) == 0)
  {
    // Log has not wrapped yet
    streamWrite(chp, g_logbuffer, pos);
  }
  else
  {
    size_t size_after_wrap = (pos & LOG_MASK);
    size_t size_before_wrap = LOG_SIZE - size_after_wrap;
    streamWrite(chp, g_logbuffer + (pos & LOG_MASK), size_before_wrap); 
    streamWrite(chp, g_logbuffer, size_after_wrap);
  }
}

void vdbg(const char *fmt, va_list ap)
{
  syssts_t state = chSysGetStatusAndLockX();
  chprintf(&g_logstream, "[%08u] ", (unsigned)chVTGetSystemTimeX());
  chvprintf(&g_logstream, fmt, ap);
  chprintf(&g_logstream, "\n");
  chSysRestoreStatusX(state);
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
  
  va_list ap;
  va_start(ap, fmt);
  vdbg(fmt, ap);
  va_end(ap);
  while(1);
}

uint32_t *HARDFAULT_PSP;

void __attribute__((naked)) HardFault_Handler(void)
{
    asm("mrs %0, psp" : "=r"(HARDFAULT_PSP) : :);
    abort_with_error("HF %08x %08x", HARDFAULT_PSP[6], SCB->CFSR);
}

void abort(void)
{
  abort_with_error("abort");
  while(1);
}

#define CPU_IDLE_THRESHOLD 64
static uint32_t g_cpu_usage_prevtime;
static float g_cpu_usage;

int get_cpu_usage(void)
{
  float usage = g_cpu_usage;
  uint32_t time = chSysGetRealtimeCounterX();
  uint32_t delta = time - g_cpu_usage_prevtime;
  
  if (delta > CPU_IDLE_THRESHOLD)
  {
    // Add the usage since the last update
    usage += (1 - usage) * delta * (1.0f / CPU_FREQ);
  }

  return (int)(usage * 100.0f);
}

void cpu_usage_idle_loop(void)
{
  uint32_t time = chSysGetRealtimeCounterX();
  uint32_t delta = time - g_cpu_usage_prevtime;
  g_cpu_usage_prevtime = time;
    
  if (delta < CPU_IDLE_THRESHOLD)
  {
    // CPU was idle during this period
    // Exponential decay over CPU_FREQ cycles = 1 second
    g_cpu_usage -= g_cpu_usage * delta * (1.0f / CPU_FREQ);
  }
  else
  {
    // CPU was in use during this period
    g_cpu_usage += (1 - g_cpu_usage) * delta * (1.0f / CPU_FREQ);
  }
}
