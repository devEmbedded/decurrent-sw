#include <ch.h>
#include <assert.h>
#include "databuf.h"

static guarded_memory_pool_t g_databuf_pool_cpu;
static guarded_memory_pool_t g_databuf_pool_d1;
static guarded_memory_pool_t g_databuf_pool_d2;
static guarded_memory_pool_t g_databuf_pool_d3;

#define DTCM_OBJCOUNT 3
#define D1_OBJCOUNT (512*1024 / DATABUF_BYTES - 1)
#define D2_OBJCOUNT (288*1024 / DATABUF_BYTES)
#define D3_OBJCOUNT (64*1024 / DATABUF_BYTES)

static databuf_t g_databuf_dtcm[DTCM_OBJCOUNT];

__attribute__((section(".ram0"), aligned (32)))
static databuf_t g_databuf_d1[D1_OBJCOUNT];

__attribute__((section(".ram2"), aligned (32)))
static databuf_t g_databuf_d2[D2_OBJCOUNT];

__attribute__((section(".ram4"), aligned (32)))
static databuf_t g_databuf_d3[D3_OBJCOUNT];

void databuf_init()
{
    chGuardedPoolObjectInit(&g_databuf_pool_cpu, DATABUF_BYTES);
    chGuardedPoolObjectInit(&g_databuf_pool_d1, DATABUF_BYTES);
    chGuardedPoolObjectInit(&g_databuf_pool_d2, DATABUF_BYTES);
    chGuardedPoolObjectInit(&g_databuf_pool_d3, DATABUF_BYTES);

    chGuardedPoolLoadArray(&g_databuf_pool_cpu, g_databuf_dtcm, DTCM_OBJCOUNT);
    chGuardedPoolLoadArray(&g_databuf_pool_d1, g_databuf_d1, D1_OBJCOUNT);
    chGuardedPoolLoadArray(&g_databuf_pool_d2, g_databuf_d2, D2_OBJCOUNT);
    chGuardedPoolLoadArray(&g_databuf_pool_d3, g_databuf_d3, D3_OBJCOUNT);
}

int databuf_get_free_count()
{
    syssts_t state = chSysGetStatusAndLockX();
    int result = chGuardedPoolGetCounterI(&g_databuf_pool_cpu);
    result += chGuardedPoolGetCounterI(&g_databuf_pool_d1);
    result += chGuardedPoolGetCounterI(&g_databuf_pool_d2);
    result += chGuardedPoolGetCounterI(&g_databuf_pool_d3);
    chSysRestoreStatusX(state);
    return result;
}

databuf_t *databuf_try_allocate(databuf_domain_t domain)
{
    syssts_t state = chSysGetStatusAndLockX();
    databuf_t *result = NULL;

    switch(domain)
    {
        case DOMAIN_CPU: result = chGuardedPoolAllocI(&g_databuf_pool_cpu); break;
        case DOMAIN_D1: result = chGuardedPoolAllocI(&g_databuf_pool_d1); break;
        case DOMAIN_D2: result = chGuardedPoolAllocI(&g_databuf_pool_d2); break;
        case DOMAIN_D3: result = chGuardedPoolAllocI(&g_databuf_pool_d3); break;
    }
    
    if (!result) result = chGuardedPoolAllocI(&g_databuf_pool_d1);
    if (!result) result = chGuardedPoolAllocI(&g_databuf_pool_d2);
    if (!result) result = chGuardedPoolAllocI(&g_databuf_pool_d3);
    
    chSysRestoreStatusX(state);
    return result;
}

databuf_t *databuf_allocate(databuf_domain_t domain)
{
    databuf_t *result = databuf_try_allocate(domain);
    
    while (!result)
    {
        // Wait on the preferred pool with timeout.
        // This doesn't react as fast for released buffers on other pools, but is good enough in practice.
        // The waiting functionality is used only when capture ends due to lack of buffers, in which case
        // the processing threads will wait here until USB thread frees up some buffers.

        switch(domain)
        {
            case DOMAIN_CPU: result = chGuardedPoolAllocTimeout(&g_databuf_pool_cpu, TIME_MS2I(10)); break;
            case DOMAIN_D1: result = chGuardedPoolAllocTimeout(&g_databuf_pool_d1, TIME_MS2I(10)); break;
            case DOMAIN_D2: result = chGuardedPoolAllocTimeout(&g_databuf_pool_d2, TIME_MS2I(10)); break;
            case DOMAIN_D3: result = chGuardedPoolAllocTimeout(&g_databuf_pool_d3, TIME_MS2I(10)); break;
        }
        
        // Check if there is anything free on other pools
        if (!result) result = databuf_try_allocate(domain);
    }

    return result;
}

void databuf_release(databuf_t **buf)
{
    if (*buf)
    {
        syssts_t state = chSysGetStatusAndLockX();
        switch (databuf_get_domain(*buf))
        {
            case DOMAIN_CPU: chGuardedPoolFreeI(&g_databuf_pool_cpu, *buf); break;
            case DOMAIN_D1: chGuardedPoolFreeI(&g_databuf_pool_d1, *buf); break;
            case DOMAIN_D2: chGuardedPoolFreeI(&g_databuf_pool_d2, *buf); break;
            case DOMAIN_D3: chGuardedPoolFreeI(&g_databuf_pool_d3, *buf); break;
        }
        *buf = NULL;
        chSysRestoreStatusX(state);
    }
}

databuf_domain_t databuf_get_domain(databuf_t *buf)
{
    uint8_t segment = ((uint32_t)buf) >> 24;
    switch (segment)
    {
        case 0x20: return DOMAIN_CPU;
        case 0x24: return DOMAIN_D1;
        case 0x30: return DOMAIN_D2;
        case 0x38: return DOMAIN_D3;
        default: assert(false); return DOMAIN_D1;
    }
}
