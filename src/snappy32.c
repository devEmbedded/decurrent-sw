#include "snappy32.h"
#include <stdbool.h>

#define SNAPPY_HASH(x) (((((uint64_t)(x) * 0x4f2c0c44UL) >> 32) + x) & (SNAPPY_HISTORY_SIZE - 1))
#define SNAPPY_MAX_UNCOMPRESSED_LEN ((1 << (7*3)) - 1)
#define SNAPPY_RUNOVER_LEN 16

static uint32_t dummy_preprocessor(uint32_t x)
{
    return x;
}

void snappy_init(snappy_state_t *state, snappy_preprocessor_t preprocessor, uint8_t *buffer, size_t buflen)
{
    state->startpos = buffer;
    state->wrpos = buffer + 3; // Reserve space for length prefix
    state->endpos = buffer + buflen - SNAPPY_RUNOVER_LEN; // Reserve some extra space so that we can check boundary less often
    state->uncompressed_len = 0;
    
    if (preprocessor != NULL)
    {
        state->preprocessor = preprocessor;
    }
    else
    {
        state->preprocessor = dummy_preprocessor;
    }
}

// Returns true if all 16 words in data equal x.
__attribute__((optimize("O2")))
bool compare_16words(uint32_t *data, uint32_t x)
{
    uint32_t diff;
    diff = data[0] ^ x;
    diff |= data[7] ^ x;
    diff |= data[15] ^ x;
    if (diff) return false;
    diff |= data[1] ^ x;
    diff |= data[2] ^ x;
    diff |= data[3] ^ x;
    diff |= data[4] ^ x;
    diff |= data[5] ^ x;
    diff |= data[6] ^ x;
    diff |= data[8] ^ x;
    diff |= data[9] ^ x;
    diff |= data[10] ^ x;
    diff |= data[11] ^ x;
    diff |= data[12] ^ x;
    diff |= data[13] ^ x;
    diff |= data[14] ^ x;
    return (diff == 0);
}

__attribute__((optimize("O2")))
size_t snappy_compress(snappy_state_t *state, uint32_t *data, size_t wordcount)
{
    if (state->uncompressed_len + wordcount * 4 > SNAPPY_MAX_UNCOMPRESSED_LEN)
    {
        return 0;
    }

    size_t index = 0;
    uint32_t prev_word = ~data[0];
    uint8_t *wrpos = state->wrpos;
    uint8_t *literal_start = NULL;
    int literal_len = 0;
    const uint8_t *endpos = state->endpos;

    while (index < wordcount && wrpos < endpos)
    {
        uint32_t word = data[index++];
        
        if (word == prev_word)
        {
            // Ok, directly repeats the previous word
            // Find repeat length
            if (index < wordcount && data[index] == prev_word)
            {
                if (index + 1 < wordcount && data[index + 1] == prev_word)
                {
                    size_t start = index;
                    index += 2;
                    if (index + 13 < wordcount)
                    {
                        if (data[index++] == prev_word) // replen = 3
                            if (data[index++] == prev_word) // replen = 4
                                if (data[index++] == prev_word) // replen = 5
                                    if (data[index++] == prev_word) // replen = 6
                                        if (data[index++] == prev_word) // replen = 7
                                            if (data[index++] == prev_word) // replen = 8
                                                if (data[index++] == prev_word) // replen = 9
                                                    if (data[index++] == prev_word) // replen = 10
                                                        if (data[index++] == prev_word) // replen = 11
                                                            if (data[index++] == prev_word) // replen = 12
                                                                if (data[index++] == prev_word) // replen = 13
                                                                    if (data[index++] == prev_word) // replen = 14
                                                                        if (data[index++] == prev_word) // replen = 15
                                                                            index++;
                    }
                    index -= 1;
                    size_t replen = index - start;
                    
                    // "Copy with 2-byte offset" 0bFFFFFFFF FFFFFFFF LLLLLL10
                    // Offset F is always 4 in this branch.
                    // Length L is repeat length - 1, in bytes
                    *(uint32_t*)wrpos = 0x00040E | (replen << 4);
                    wrpos += 3;

                    if (replen == 15 && index + 16 <= wordcount && compare_16words(data + index, prev_word))
                    {
                        // Further 16 words of repeat
                        *(uint32_t*)wrpos = 0x0004FE;
                        wrpos += 3;
                        index += 16;

                        // Check how long this repeat continues
                        // This is faster than going through the main loop for long repeats.
                        int maxcount = (wordcount - index) / 16;
                        if (maxcount > (endpos - wrpos) / 3) maxcount = (endpos - wrpos) / 3;
                        while (maxcount-- > 0 && compare_16words(data + index, prev_word))
                        {
                            *(uint32_t*)wrpos = 0x0004FE;
                            wrpos += 3;
                            index += 16;
                        }
                    }
                }
                else
                {
                    // "Copy with 1-byte offset" 0bFFFFFFFF FFFLLL01
                    // Offset F is always 4 in this branch.
                    // Length L is always 4 in this branch, meaning 8 bytes
                    *(uint16_t*)wrpos = 0x0411;
                    wrpos += 2;
                    index += 1;
                }
            }
            else
            {
                // "Copy with 1-byte offset" 0bFFFFFFFF FFFLLL01
                // Offset F is always 4 in this branch.
                // Length L is always 0 in this branch, meaning 4 bytes
                *(uint16_t*)wrpos = 0x0401;
                wrpos += 2;
            }
        }
        else
        {
            prev_word = word;

            // Check for repeats further back in the history
            uint32_t hashkey = SNAPPY_HASH(word);
            uint16_t offset = state->history[hashkey];
            state->history[hashkey] = index - 1;

            if (offset < index - 1 && data[offset] == word)
            {
                uint32_t delta = index - offset - 1;

                if (data[index] == data[offset + 1])
                {
                    prev_word = data[index];
                    if (data[index + 1] == data[offset + 2])
                    {
                        // Ok, repeat of at least 3 words, find out how many
                        int replen = 2;
                        while (data[index + replen] == data[offset + replen + 1] && replen < 15)
                        {
                            replen++;
                        }
                        
                        index += replen;
                        prev_word = data[index - 1];

                        // "Copy with 2-byte offset" 0bFFFFFFFF FFFFFFFF LLLLLL10
                        // Offset F is up to 65532 in this branch
                        // Length L is repeat length - 1, in bytes
                        *(uint32_t*)wrpos = 0x00000E | (replen << 4) | (delta << 10);
                        wrpos += 3;
                    }
                    else if (delta < 512)
                    {
                        // Repeat of 2 words
                        // "Copy with 1-byte offset" 0bFFFFFFFF FFFLLL01
                        // Offset F is up to 2044 bytes in this branch
                        // Length L is always 4 in this branch, meaning 8 bytes
                        *(uint16_t*)wrpos = 0x0011 | (delta << 10) | ((delta >> 6) << 5);
                        wrpos += 2;
                        index += 1;
                    }
                    else
                    {
                        // Repeat of 2 words
                        // "Copy with 2-byte offset" 0bFFFFFFFF FFFFFFFF LLLLLL10
                        // Offset F is up to 65532 in this branch
                        // Length L is always 7 in this branch, meaning 8 bytes
                        *(uint32_t*)wrpos = 0x00001E | (delta << 10);
                        wrpos += 3;
                        index += 1;
                    }
                }
                else if (delta < 512)
                {
                    // Repeat of 1 word
                    // "Copy with 1-byte offset" 0bFFFFFFFF FFFLLL01
                    // Offset F is up to 2044 bytes in this branch
                    // Length L is always 0 in this branch, meaning 4 bytes
                    *(uint16_t*)wrpos = 0x0001 | (delta << 10) | ((delta >> 6) << 5);
                    wrpos += 2;
                }
                else
                {
                    // Repeat of 1 word
                    // "Copy with 2-byte offset" 0bFFFFFFFF FFFFFFFF LLLLLL10
                    // Offset F is up to 65532 in this branch
                    // Length L is always 3 in this branch, meaning 4 bytes
                    *(uint32_t*)wrpos = 0x00000E | (delta << 10);
                    wrpos += 3;
                }
            }
            else
            {
                // Have to encode as literal block
                uint32_t pword = state->preprocessor(word);
                
                if (literal_len > 0)
                {
                    // Append to existing literal block
                    *(uint32_t*)wrpos = pword;
                    wrpos += 4;
                    literal_len += 4;

                    if (literal_len == 60)
                    {
                        // Flush this literal block now
                        *literal_start = (literal_len - 1) << 2;
                        literal_len = 0;
                    }

                    continue;
                }
                else
                {
                    // Start new literal block
                    literal_start = wrpos;
                    wrpos += 1; // Reserve space for block start tag
                    *(uint32_t*)wrpos = pword;
                    wrpos += 4;
                    literal_len = 4;
                    continue; // No need to flush this block yet
                }
            }
        }

        if (literal_len != 0)
        {
            // We encoded something other than a literal block, so terminate the previous block.
            *literal_start = (literal_len - 1) << 2;
            literal_len = 0;
        }
    }

    if (literal_len != 0)
    {
        // Terminate last literal block
        *literal_start = (literal_len - 1) << 2;
        literal_len = 0;
    }

    state->wrpos = wrpos;
    state->uncompressed_len += index * 4;

    return index;
}

size_t snappy_finish(snappy_state_t *state)
{
    // Encode total length of uncompressed data
    state->startpos[0] = ((state->uncompressed_len >>  0) & 0x7F) | 0x80;
    state->startpos[1] = ((state->uncompressed_len >>  7) & 0x7F) | 0x80;
    state->startpos[2] = ((state->uncompressed_len >> 14) & 0x7F);
    
    return state->wrpos - state->startpos;
}