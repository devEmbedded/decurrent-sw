// Shared pool of buffers for signal capture

#pragma once

#include <stdint.h>

#define DATABUF_WORDS 4096
#define DATABUF_BYTES (4 * DATABUF_WORDS)

typedef struct {
    uint32_t data[DATABUF_WORDS];
} databuf_t;

// Initialize buffer pool
void databuf_init(void);

// Returns number of free buffers in pool
// Can be called from interrupts and threads.
int databuf_get_free_count(void);

// Allocate buffer from the shared pool, returns NULL if not available.
// Can be called from interrupts and threads.
databuf_t *databuf_try_allocate(void);

// Allocate buffer from the shared pool, wait if not available.
// Can be called from threads only
databuf_t *databuf_allocate(void);

// Release buffer back to pool
// Can be called from interrupts and threads.
// If *buf is already NULL, does nothing.
void databuf_release(databuf_t **buf);
