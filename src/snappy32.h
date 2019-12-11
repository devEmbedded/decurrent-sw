// Compression algorithm compatible with Google's Snappy format.
// Processes input at 32 bits at a time, to optimize speed for Cortex-M7.

#pragma once

#include <stdint.h>
#include <stdlib.h>

#define SNAPPY_HISTORY_SIZE 1024

// Pointer to a transformation function that is optionally applied to each
// word before encoding. It must be idempotent and produce same result every
// time with the same argument.
typedef uint32_t (*snappy_preprocessor_t)(uint32_t x);

typedef struct {
    uint8_t *startpos;
    uint8_t *wrpos;
    uint8_t *endpos;
    uint32_t uncompressed_len;
    snappy_preprocessor_t preprocessor;
    uint16_t history[SNAPPY_HISTORY_SIZE];
} snappy_state_t;

// Initialize for writing compressed data into buffer.
void snappy_init(snappy_state_t *state, snappy_preprocessor_t preprocessor, uint8_t *buffer, size_t buflen);

// Compress some data. Can be called multiple times.
// Compresses up to wordcount on input, or until output buffer is nearly full.
// Returns number of words read from input.
size_t snappy_compress(snappy_state_t *state, uint32_t *data, size_t wordcount);

// Finish compressed stream.
// Returns total number of bytes that has been written to output buffer.
size_t snappy_finish(snappy_state_t *state);