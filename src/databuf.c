#include <ch.h>
#include "databuf.h"

static guarded_memory_pool_t g_databuf_pool;

#define RAM0_OBJCOUNT (512*1024 / DATABUF_BYTES)
#define RAM1_OBJCOUNT (256*1024 / DATABUF_BYTES)

__attribute__((section(".ram0")))
static databuf_t g_databuf_ram0[RAM0_OBJCOUNT];

__attribute__((section(".ram1")))
static databuf_t g_databuf_ram1[RAM1_OBJCOUNT];

void databuf_init()
{
    chGuardedPoolObjectInit(&g_databuf_pool, DATABUF_BYTES);
    chGuardedPoolLoadArray(&g_databuf_pool, g_databuf_ram0, RAM0_OBJCOUNT);
    chGuardedPoolLoadArray(&g_databuf_pool, g_databuf_ram1, RAM1_OBJCOUNT);
}

int databuf_get_free_count()
{
    syssts_t state = chSysGetStatusAndLockX();
    int result = chGuardedPoolGetCounterI(&g_databuf_pool);
    chSysRestoreStatusX(state);
    return result;
}

databuf_t *databuf_try_allocate()
{
    syssts_t state = chSysGetStatusAndLockX();
    databuf_t *result = chGuardedPoolAllocI(&g_databuf_pool);
    chSysRestoreStatusX(state);
    return result;
}

databuf_t *databuf_allocate()
{
    return chGuardedPoolAllocTimeout(&g_databuf_pool, TIME_INFINITE);
}

void databuf_release(databuf_t *buf)
{
    syssts_t state = chSysGetStatusAndLockX();
    chGuardedPoolFreeI(&g_databuf_pool, buf);
    chSysRestoreStatusX(state);
}
