// RFC1951/DEFLATE compliant implementation of Huffman coding.
// This module implements constructing the code from a list of code lengths.

#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// This encoder does not use code index 285
#define HUFFMAN_MAX_CODE_BITS 15
#define HUFFMAN_CODE_COUNT 285
#define HUFFMAN_MAX_REPEAT_LEN 257

// Deflate cache will use combined codes, huffman_code_t has space for this
// many bits total.
#define COMBINED_CODE_BITS 27

typedef struct {
    uint32_t buf;
    unsigned bits_in_buf;
    uint32_t *wr_pos;
    uint32_t *end_pos;
} bitstream_t;

// Top 5 bits encode length, bottom 27 bits are the code
typedef struct {
    unsigned bits : 27;
    unsigned len : 5;
} huffman_code_t;

typedef struct {
    huffman_code_t codes[HUFFMAN_CODE_COUNT];

    // This encoder only uses distance codes 0 and 3 (lookback distances 1 and 4)
    huffman_code_t distance_1;
    huffman_code_t distance_4;
} huffman_alphabet_t;

static inline void bitstream_init(bitstream_t *stream, uint32_t *buffer, size_t wordcount)
{
    stream->buf = 0;
    stream->bits_in_buf = 0;
    stream->wr_pos = buffer;
    stream->end_pos = buffer + wordcount;
}

// Returns false when buffer is full
static inline bool bitstream_write(bitstream_t *stream, huffman_code_t code)
{
    unsigned bits_in_buf = stream->bits_in_buf;
    stream->buf |= code.bits << stream->bits_in_buf;
    bits_in_buf += code.len;

    if (bits_in_buf < 32)
    {
        stream->bits_in_buf = bits_in_buf;
        return true;
    }
    else
    {
        *stream->wr_pos = stream->buf;
        stream->wr_pos++;
        stream->bits_in_buf = bits_in_buf - 32;
        stream->buf = code.bits >> (code.len - bits_in_buf);

        return (stream->wr_pos < stream->end_pos);
    }

    
}

static inline huffman_code_t huffman_code_append(huffman_code_t code, huffman_code_t other)
{
    code.bits |= other.bits << code.len;
    code.len += other.len;
    return code;
}

// Create Huffman prefix code based on symbol frequencies.
// The alphabet should be initialized to have the frequencies in 'bits' field
// of each code.
void huffman_create_alphabet(huffman_alphabet_t *alphabet);

// Write out the alphabet description in deflate format.
void huffman_write_alphabet(bitstream_t *stream, huffman_alphabet_t *alphabet);

