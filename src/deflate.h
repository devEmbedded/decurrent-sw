// RFC1951/DEFLATE compliant compression algorithm.
// This module implements run length encoding part and uses Huffman-module
// to build the alphabet for encoding literals.

#pragma once
#include "huffman.h"
#include <stdbool.h>

#define DEFLATE_CACHE_SIZE 1024
#define DEFLATE_HASH(x) (((((uint64_t)(x) * 0x4f2c0c44UL) >> 32) + x) & (DEFLATE_CACHE_SIZE - 1))

typedef struct {
    uint32_t literal;
    huffman_code_t code;
} deflate_cache_entry_t;

typedef struct {
    bitstream_t stream;
    huffman_alphabet_t alphabet;
    deflate_cache_entry_t cache[DEFLATE_CACHE_SIZE];
} deflate_state_t;

// Initialize deflate dictionary, using the data to compute the symbol frequencies.
void deflate_init(deflate_state_t *state, uint32_t *data, size_t wordcount);

// Start writing a deflate block to buffer
void deflate_start_block(deflate_state_t *state, uint32_t *buffer);

// Compress some data. Can be called multiple times.
// Note that this doesn't perform any checking for the remaining space in buffer,
// caller should check state->stream->wr_pos to ensure enough space remains.
// If nibble_swap is true, swaps 0x76543210 to 0x75643120 to account for QuadSPI data order.
void deflate_compress(deflate_state_t *state, uint32_t *data, size_t wordcount, bool nibble_swap);

// Finish a deflate block and flush the bitstream buffer
void deflate_end_block(deflate_state_t *state);
