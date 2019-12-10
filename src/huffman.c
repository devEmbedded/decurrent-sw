#include "huffman.h"

#define NO_NODE 0xFFFF
#define HEAP_SIZE (HUFFMAN_CODE_COUNT * 2)
#define HEAP_PARENT(x) ((x-1)/2)
#define HEAP_LEFTCHILD(x) (2*x + 1)
#define HEAP_RIGHTCHILD(x) (2*x + 2)
#define HEAP_SWAP(a, b) do {uint16_t tmp = a; a = b; b = tmp;} while(0)

typedef struct {
    uint16_t weight; // Sum of weights for this branch
    uint16_t parent; // Index of parent node
} node_t;

// Insert new element in heap and "sift up" other nodes to maintain order.
__attribute__((optimize("O2")))
static void heap_insert(const node_t *nodes, uint16_t *minheap, int *heap_size, uint16_t element)
{
    minheap[(*heap_size)++] = element;

    int child = *heap_size - 1;
    while (child > 0)
    {
        int parent = HEAP_PARENT(child);
        if (nodes[minheap[parent]].weight > nodes[minheap[child]].weight)
        {
            HEAP_SWAP(minheap[parent], minheap[child]);
            child = parent;
        }
        else
        {
            break;
        }
    }
}

// Return smallest element in heap and "sift down" other nodes.
__attribute__((optimize("O2")))
static uint16_t heap_remove(const node_t *nodes, uint16_t *minheap, int *heap_size)
{
    uint16_t result = minheap[0];
    minheap[0] = minheap[*heap_size - 1];
    *heap_size -= 1;
    
    int root = 0;
    int size = *heap_size;
    while (HEAP_LEFTCHILD(root) < size)
    {
        int left = HEAP_LEFTCHILD(root);
        int right = HEAP_RIGHTCHILD(root);
        if (right >= size) right = left;
        int wroot = nodes[minheap[root]].weight;
        int wleft = nodes[minheap[left]].weight;
        int wright = nodes[minheap[right]].weight;

        if (wroot < wleft && wroot < wright)
        {
            break;
        }        
        else if (wleft < wright)
        {
            HEAP_SWAP(minheap[root], minheap[left]);
            root = left;
        }
        else
        {
            HEAP_SWAP(minheap[root], minheap[right]);
            root = right;
        }
    }
    
    return result;
}

// Assumes each symbol has occurrence count in 'bits' field.
// Fills in length for each symbol.
// Algorithm from https://en.wikipedia.org/wiki/Huffman_coding#Compression
__attribute__((optimize("O2")))
static void calculate_lengths_from_counts(huffman_alphabet_t *alphabet)
{
    // First HUFFMAN_CODE_COUNT entries are leafs, rest are intermediate nodes.
    node_t nodes[HEAP_SIZE];
    uint16_t minheap[HEAP_SIZE];
    int node_count = HUFFMAN_CODE_COUNT;
    int heap_size = 0;
    
    // Fill in the leafs and construct heap
    for (int i = 0; i < HUFFMAN_CODE_COUNT; i++)
    {
        nodes[i].weight = alphabet->codes[i].bits;
        nodes[i].parent = NO_NODE;
        
        if (nodes[i].weight > 0)
        {
            heap_insert(nodes, minheap, &heap_size, i);
        }
    }
    
    // Combine two least probable symbols
    while (heap_size > 1)
    {
        uint16_t node1 = heap_remove(nodes, minheap, &heap_size);
        uint16_t node2 = heap_remove(nodes, minheap, &heap_size);
        
        int combined = node_count++;
        nodes[combined].weight = nodes[node1].weight + nodes[node2].weight;
        nodes[combined].parent = NO_NODE;
        nodes[node1].parent = combined;
        nodes[node2].parent = combined;
        
        heap_insert(nodes, minheap, &heap_size, combined);
    }
    
    // Calculate lengths for each symbol
    int count_too_long_codes = 0;
    for (int i = 0; i < HUFFMAN_CODE_COUNT; i++)
    {
        int len = 0;
        int node = i;
        while (nodes[node].parent != NO_NODE)
        {
            len++;
            node = nodes[node].parent;
        }
        
        if (len > HUFFMAN_MAX_CODE_BITS)
        {
            count_too_long_codes++;
            len = HUFFMAN_MAX_CODE_BITS;
        }
        
        alphabet->codes[i].len = len;
    }
    
    // Balance symbol lengths in order not to exceed maximum code length
    while (count_too_long_codes > 0)
    {
        int longest = 0;
        for (int i = 1; i < HUFFMAN_CODE_COUNT; i++)
        {
            if (alphabet->codes[i].len < HUFFMAN_MAX_CODE_BITS &&
                alphabet->codes[i].len > alphabet->codes[longest].len)
            {
                longest = i;
                
                if (alphabet->codes[i].len == HUFFMAN_MAX_CODE_BITS - 1)
                {
                    break;
                }
            }
            
            while (count_too_long_codes > 0 && alphabet->codes[longest].len < HUFFMAN_MAX_CODE_BITS)
            {
                alphabet->codes[longest].len += 1;
                int extra_bits = HUFFMAN_MAX_CODE_BITS - alphabet->codes[longest].len;
                count_too_long_codes -= (1 << extra_bits);
            }
        }
    }
}

static inline uint32_t reverse_bits(uint32_t v)
{
#ifdef __ARM__
    asm("rbit %0, %1" : "=r" (v) : "r" (v));
#else
    v = ((v >> 1) & 0x55555555) | ((v & 0x55555555) << 1);
    v = ((v >> 2) & 0x33333333) | ((v & 0x33333333) << 2);
    v = ((v >> 4) & 0x0F0F0F0F) | ((v & 0x0F0F0F0F) << 4);
    v = ((v >> 8) & 0x00FF00FF) | ((v & 0x00FF00FF) << 8);
    v = ( v >> 16             ) | ( v               << 16);
#endif
    return v;
}

// Assumes alphabet has lengths calculated for each symbol.
// Assigns a bitstring for each symbol.
static void assign_bitstrings_to_symbols(huffman_alphabet_t *alphabet)
{
    // Algorithm from RFC1951 (DEFLATE)
    uint16_t bl_count[HUFFMAN_MAX_CODE_BITS + 1] = {0};
    
    // Count the number of codes for each code length
    for (int i = 0; i < HUFFMAN_CODE_COUNT; i++)
    {
        bl_count[alphabet->codes[i].len]++;
    }
    
    // Find the numerical value of the smallest code for each code length.
    uint32_t code = 0;
    uint32_t next_code[HUFFMAN_MAX_CODE_BITS + 1] = {0};
    bl_count[0] = 0;
    for (int len = 1; len < HUFFMAN_MAX_CODE_BITS; len++)
    {
        code = (code + bl_count[len - 1]) << 1;
        next_code[len] = code;
    }
    
    // Assign numerical values to all codes.
    for (int i = 0; i < HUFFMAN_CODE_COUNT; i++)
    {
        int len = alphabet->codes[i].len;
        if (len > 0)
        {
            alphabet->codes[i].bits = reverse_bits(next_code[len]) >> (32 - len);
            next_code[len]++;
        }
        else
        {
            alphabet->codes[i].bits = 0;
        }
    }
}

void huffman_create_alphabet(huffman_alphabet_t *alphabet)
{
    calculate_lengths_from_counts(alphabet);
    assign_bitstrings_to_symbols(alphabet);
    
    alphabet->distance_1.bits = 0;
    alphabet->distance_1.len = 1;
    alphabet->distance_4.bits = 1;
    alphabet->distance_4.len = 1;
}

static const huffman_code_t code_length_alphabet[19] = {
    (huffman_code_t){   0b000, 3}, // code len 0
    (huffman_code_t){ 0b01011, 5}, // code len 1
    (huffman_code_t){  0b0010, 4}, // code len 2
    (huffman_code_t){  0b1010, 4}, // code len 3
    (huffman_code_t){  0b0110, 4}, // code len 4
    (huffman_code_t){  0b1110, 4}, // code len 5
    (huffman_code_t){  0b0001, 4}, // code len 6
    (huffman_code_t){  0b1001, 4}, // code len 7
    (huffman_code_t){  0b0101, 4}, // code len 8
    (huffman_code_t){  0b1101, 4}, // code len 9
    (huffman_code_t){  0b0011, 4}, // code len 10
    (huffman_code_t){ 0b11011, 5}, // code len 11
    (huffman_code_t){ 0b00111, 5}, // code len 12
    (huffman_code_t){ 0b10111, 5}, // code len 13
    (huffman_code_t){0b001111, 6}, // code len 14
    (huffman_code_t){0b101111, 6}, // code len 15
    (huffman_code_t){   0b100, 3}, // 16: copy prev 3-6 times
    (huffman_code_t){0b011111, 6}, // 17: repeat 0 for 3-10 times
    (huffman_code_t){0b111111, 6}, // 18: repeat 0 for 11-138 times
};

#include <stdio.h>
void huffman_write_alphabet(bitstream_t *stream, huffman_alphabet_t *alphabet)
{
    // 5 bits: number of literal/length codes - 257
    bitstream_write(stream, (huffman_code_t){HUFFMAN_CODE_COUNT - 257, 5});
    
    // 5 bits: number of distance codes - 1
    bitstream_write(stream, (huffman_code_t){4 - 1, 5});
    
    // 4 bits: number of code length codes - 4
    bitstream_write(stream, (huffman_code_t){19 - 4, 4});
    
    // 19x3 bits: code lengths for code length alphabet
    static const uint8_t zigzag[19] = {16, 17, 18, 0, 8, 7, 9, 6, 10, 5,
                                       11, 4, 12, 3, 13, 2, 14, 1, 15};
    for (int i = 0; i < 19; i++)
    {
        int j = zigzag[i];
        bitstream_write(stream, (huffman_code_t){code_length_alphabet[j].len, 3});
    }
    
    // Literal / length alphabet lengths
    for (int i = 0; i < HUFFMAN_CODE_COUNT; i++)
    {
        int len = alphabet->codes[i].len;
        int repeats = 1;
        while (i + repeats < HUFFMAN_CODE_COUNT
               && alphabet->codes[i + repeats].len == len)
        {
            repeats++;
        }
        
        if (repeats < 3)
        {
            bitstream_write(stream, code_length_alphabet[len]);
        }
        else if (len == 0 && repeats < 11)
        {
            bitstream_write(stream, code_length_alphabet[17]);
            bitstream_write(stream, (huffman_code_t){repeats - 3, 3});
            i += repeats - 1;
        }
        else if (len == 0)
        {
            if (repeats > 138) repeats = 138;
            bitstream_write(stream, code_length_alphabet[18]);
            bitstream_write(stream, (huffman_code_t){repeats - 11, 7});
            i += repeats - 1;
        }
        else
        {
            bitstream_write(stream, code_length_alphabet[len]);
            repeats -= 1;
            
            while (repeats >= 9)
            {
                bitstream_write(stream, code_length_alphabet[16]);
                bitstream_write(stream, (huffman_code_t){3, 2});
                i += 6;
                repeats -= 6;
            }
            
            if (repeats > 6)
            {
                bitstream_write(stream, code_length_alphabet[16]);
                bitstream_write(stream, (huffman_code_t){0, 2});
                i += 3;
                repeats -= 3;
            }
            
            if (repeats > 3)
            {
                bitstream_write(stream, code_length_alphabet[16]);
                bitstream_write(stream, (huffman_code_t){repeats - 3, 2});
                i += repeats;
            }
        }
    }
    
    // Distance alphabet lengths
    // This encoder uses only distance codes 0 and 3.
    bitstream_write(stream, code_length_alphabet[1]);
    bitstream_write(stream, code_length_alphabet[0]);
    bitstream_write(stream, code_length_alphabet[0]);
    bitstream_write(stream, code_length_alphabet[1]);
}

