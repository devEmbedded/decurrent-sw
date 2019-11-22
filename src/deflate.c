#include "deflate.h"

static inline uint16_t repeat_length_code_index(int length)
{
    if (length < 11)
        return 257 + (length - 3);
    else if (length < 19)
        return 265 + (length - 11) / 2;
    else if (length < 35)
        return 269 + (length - 19) / 4;
    else if (length < 67)
        return 273 + (length - 35) / 8;
    else if (length < 131)
        return 277 + (length - 67) / 16;
    else
        return 281 + (length - 131) / 32;
}

static inline huffman_code_t repeat_length_code(huffman_alphabet_t *alphabet, int length)
{
    huffman_code_t code = alphabet->codes[repeat_length_code_index(length)];
    
    if (length < 11)
        return code;
    else if (length < 19)
        return huffman_code_append(code, (huffman_code_t){(length - 11) & 1, 1});
    else if (length < 35)
        return huffman_code_append(code, (huffman_code_t){(length - 19) & 3, 2});
    else if (length < 67)
        return huffman_code_append(code, (huffman_code_t){(length - 35) & 7, 3});
    else if (length < 131)
        return huffman_code_append(code, (huffman_code_t){(length - 67) & 15, 4});
    else
        return huffman_code_append(code, (huffman_code_t){(length - 131) & 31, 5});
}

void deflate_init(deflate_state_t *state, uint32_t *data, size_t wordcount)
{
    // Clear alphabet and cache
    // We assign '1' as initial count of each literal to ensure that all byte
    // values can be encoded.
    for (int i = 0; i < HUFFMAN_CODE_COUNT; i++)
    {
        if (i <= 256)
        {
            state->alphabet.codes[i].bits = 1;
        }
        else
        {
            state->alphabet.codes[i].bits = 0;
        }
    }
    
    // Assign invalid value to each cache entry
    for (int i = 0; i < DEFLATE_CACHE_SIZE; i++)
    {
        state->cache[i].literal = 0;
    }
    
    // Assign 0xFFFF to entry for 0 to ensure that it also is invalid
    state->cache[DEFLATE_HASH(0)].literal = 0xFFFF;
    
    // We only use length codes 3, and all multiples of 4.
    state->alphabet.codes[repeat_length_code_index(3)].bits = 1;
    for (int i = 4; i < HUFFMAN_MAX_REPEAT_LEN; i += 4)
    {
        state->alphabet.codes[repeat_length_code_index(i)].bits = 1;
    }
        
    // Sample symbol probabilities
    int step = wordcount / 256 + 1;
    for (size_t i = 4; i < wordcount; i += step)
    {
        uint32_t word = data[i];
        uint8_t b0 = (word >>  0) & 0xFF;
        uint8_t b1 = (word >>  8) & 0xFF;
        uint8_t b2 = (word >> 16) & 0xFF;
        uint8_t b3 = (word >> 24) & 0xFF;
                
        if (word == b0 * 0x01010101UL)
        {
            // Repeat length 3
            state->alphabet.codes[b0].bits++;
            state->alphabet.codes[257].bits++;
        }
        else
        {
            // Individual bytes
            state->alphabet.codes[b0].bits++;
            state->alphabet.codes[b1].bits++;
            state->alphabet.codes[b2].bits++;
            state->alphabet.codes[b3].bits++;
        }
        
        if (word == data[i-1])
        {
            if (word == data[i-2])
            {
                if (word == data[i-3])
                {
                    if (word == data[i-4])
                    {
                        // Repeat length 16 or more
                    }
                    else
                    {
                        // Repeat length 12
                        state->alphabet.codes[265].bits++;
                    }
                }
                else
                {
                    // Repeat length 8
                    state->alphabet.codes[262].bits++;
                }
            }
            else
            {
                // Repeat length 4
                state->alphabet.codes[258].bits++;
            }
        }
    }
    
    huffman_create_alphabet(&state->alphabet);
}

void deflate_start_block(deflate_state_t *state, uint32_t *buffer)
{
    bitstream_init(&state->stream, buffer);
    bitstream_write(&state->stream, (huffman_code_t){0b101, 3});
    huffman_write_alphabet(&state->stream, &state->alphabet);
}

// Swaps 0x76543210 to 0x75643120 to account for QuadSPI data order.
static inline uint32_t do_nibble_swap(uint32_t x)
{
    uint32_t mask = ((x ^ (x >> 4)) & 0x00F000F0) * 0x11;
    return x ^ mask;
}

void deflate_compress(deflate_state_t *state, uint32_t *data, size_t wordcount, bool nibble_swap)
{
    size_t index = 0;
    uint32_t prev_word = ~data[0];
    
    while (index < wordcount)
    {
        uint32_t word = data[index];
        index++;
        
        if (word == prev_word)
        {
            // Check how many words of repeat
            int repeat_len = 4;
            while (index < wordcount
                   && data[index] == prev_word
                   && repeat_len <= HUFFMAN_MAX_REPEAT_LEN - 4)
            {
                repeat_len += 4;
                index++;
            }
            
            // Encode repeat count
            bitstream_write(&state->stream, repeat_length_code(&state->alphabet, repeat_len));
            
            // Encode copy distance
            bitstream_write(&state->stream, state->alphabet.distance_4);
        }
        else
        {
            uint32_t hashkey = DEFLATE_HASH(word);
            
            if (state->cache[hashkey].literal == word)
            {
                // Found 4-byte word in cache
                bitstream_write(&state->stream, state->cache[hashkey].code);
            }
            else
            {
                if (nibble_swap)
                {
                    word = do_nibble_swap(word);
                }
            
                // Encode byte-by-byte
                uint8_t b0 = (word >>  0) & 0xFF;
                uint8_t b1 = (word >>  8) & 0xFF;
                uint8_t b2 = (word >> 16) & 0xFF;
                uint8_t b3 = (word >> 24) & 0xFF;
                
                if (word == b0 * 0x01010101UL)
                {
                    // All bytes are the same, use repeat len 3
                    huffman_code_t c0 = state->alphabet.codes[b0];
                    huffman_code_t c1 = state->alphabet.codes[257];
                    huffman_code_t c2 = state->alphabet.distance_1;
                    
                    if (c0.len + c1.len + c2.len <= COMBINED_CODE_BITS)
                    {
                        // Ok, combined code fits in cache, write it there.
                        huffman_code_t c = huffman_code_append(c0, c1);
                        c = huffman_code_append(c, c2);
                        bitstream_write(&state->stream, c);
                        state->cache[hashkey].literal = word;
                        state->cache[hashkey].code = c;
                    }
                    else
                    {
                        // Combined code is too long, write out directly
                        bitstream_write(&state->stream, c0);
                        bitstream_write(&state->stream, c1);
                        bitstream_write(&state->stream, c2);
                    }
                }
                else
                {
                    // Encode 4 bytes separately
                    huffman_code_t c0 = state->alphabet.codes[b0];
                    huffman_code_t c1 = state->alphabet.codes[b1];
                    huffman_code_t c2 = state->alphabet.codes[b2];
                    huffman_code_t c3 = state->alphabet.codes[b3];
                    
                    if (c0.len + c1.len + c2.len + c3.len <= COMBINED_CODE_BITS)
                    {
                        // Ok, combined code fits in cache, write it there.
                        huffman_code_t c = huffman_code_append(c0, c1);
                        c = huffman_code_append(c, c2);
                        c = huffman_code_append(c, c3);
                        bitstream_write(&state->stream, c);
                        state->cache[hashkey].literal = word;
                        state->cache[hashkey].code = c;
                    }
                    else
                    {
                        // Combined code is too long, write out directly
                        bitstream_write(&state->stream, c0);
                        bitstream_write(&state->stream, c1);
                        bitstream_write(&state->stream, c2);
                        bitstream_write(&state->stream, c3);
                    }
                }
            }
        }
        
        prev_word = word;
    }
}

void deflate_end_block(deflate_state_t *state)
{
    bitstream_write(&state->stream, state->alphabet.codes[256]);
    bitstream_write(&state->stream, (huffman_code_t){0, 32 - state->stream.bits_in_buf});
}

