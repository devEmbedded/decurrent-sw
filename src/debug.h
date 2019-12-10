// Provides hardfault handler and a rolling log buffer

#pragma once

#include <stdarg.h>

struct BaseSequentialStream;
void debug_print_log(BaseSequentialStream *chp);

void vdbg(const char *fmt, va_list ap);
void dbg(const char *fmt, ...);

void abort_with_error(const char *fmt, ...);

int get_cpu_usage(void);
void cpu_usage_idle_loop(void);
