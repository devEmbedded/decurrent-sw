// Shared pool of buffers for signal capture

#pragma once

#include <stdint.h>
#include <stdbool.h>

#define DATABUF_WORDS 4096
#define DATABUF_BYTES (4 * DATABUF_WORDS)
#define DATABUF_COUNT (1024 * 1024 / DATABUF_BYTES)

typedef struct {
    uint32_t data[DATABUF_WORDS];
} databuf_t;

typedef enum {
    DOMAIN_CPU = 0, // DTCM memory, accessible by CPU and MDMA
    DOMAIN_D1 = 1, // D1 AXI SRAM, accessible by any DMA controller, needs cache invalidation by CPU
    DOMAIN_D2 = 2, // D2 SRAM, accessible by any DMA controller, needs cache invalidation by CPU
    DOMAIN_D3 = 3  // D3 SRAM, accessible by any DMA controller, needs cache invalidation by CPU
} databuf_domain_t;

// Initialize buffer pool
void databuf_init(void);

// Returns number of free buffers in pool
// Can be called from interrupts and threads.
int databuf_get_free_count(void);

// Allocate buffer from the pool, returns NULL if not available.
// Can be called from interrupts and threads.
// Caller can specify preferred domain, allocation will fall back to D1/D2/D3 domains in that order.
databuf_t *databuf_try_allocate(databuf_domain_t domain);

// Allocate buffer from the pool, wait if not available.
// Can be called from threads only
// Caller can specify preferred domain, allocation will fall back to D1/D2/D3 domains in that order.
databuf_t *databuf_allocate(databuf_domain_t domain);

// Release buffer back to pool
// Can be called from interrupts and threads.
// If *buf is already NULL, does nothing.
void databuf_release(databuf_t **buf);

// Returns the domain the buffer is allocated from.
databuf_domain_t databuf_get_domain(databuf_t *buf);
