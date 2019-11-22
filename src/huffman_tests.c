#include "unittests.h"
#include "huffman.c"

int main()
{
    int status = 0;

    {
        // Example code from RFC1951
        COMMENT("Test assign_bitstrings_to_symbols");
        huffman_alphabet_t alphabet = {};
        alphabet.codes[0].len = 3;
        alphabet.codes[1].len = 3;
        alphabet.codes[2].len = 3;
        alphabet.codes[3].len = 3;
        alphabet.codes[4].len = 3;
        alphabet.codes[5].len = 2;
        alphabet.codes[6].len = 4;
        alphabet.codes[7].len = 4;
    
        assign_bitstrings_to_symbols(&alphabet);
        TEST(alphabet.codes[0].bits == 0b010)
        TEST(alphabet.codes[1].bits == 0b110)
        TEST(alphabet.codes[2].bits == 0b001)
        TEST(alphabet.codes[3].bits == 0b101)
        TEST(alphabet.codes[4].bits == 0b011)
        TEST(alphabet.codes[5].bits == 0b00)
        TEST(alphabet.codes[6].bits == 0b0111)
        TEST(alphabet.codes[7].bits == 0b1111)
    }
    
    {
        // Example tree from https://en.wikipedia.org/wiki/Huffman_coding
        COMMENT("Test calculate_lengths_from_counts");
        huffman_alphabet_t alphabet = {};
        alphabet.codes[' '].bits = 7;
        alphabet.codes['a'].bits = 4;
        alphabet.codes['e'].bits = 4;
        alphabet.codes['f'].bits = 3;
        alphabet.codes['h'].bits = 2;
        alphabet.codes['i'].bits = 2;
        alphabet.codes['m'].bits = 2;
        alphabet.codes['n'].bits = 2;
        alphabet.codes['s'].bits = 2;
        alphabet.codes['t'].bits = 2;
        alphabet.codes['l'].bits = 1;
        alphabet.codes['o'].bits = 1;
        alphabet.codes['p'].bits = 1;
        alphabet.codes['r'].bits = 1;
        alphabet.codes['u'].bits = 1;
        alphabet.codes['x'].bits = 1;
        
        calculate_lengths_from_counts(&alphabet);
        TEST(alphabet.codes[' '].len == 3);
        TEST(alphabet.codes['a'].len == 3);
        TEST(alphabet.codes['e'].len == 3);
        TEST(alphabet.codes['f'].len == 4);
        TEST(alphabet.codes['h'].len == 4);
        TEST(alphabet.codes['i'].len == 4);
        TEST(alphabet.codes['m'].len == 4);
        TEST(alphabet.codes['n'].len == 4);
        TEST(alphabet.codes['s'].len == 4);
        TEST(alphabet.codes['t'].len == 4);
        TEST(alphabet.codes['l'].len == 5);
        TEST(alphabet.codes['o'].len == 5);
        TEST(alphabet.codes['p'].len == 5);
        TEST(alphabet.codes['r'].len == 5);
        TEST(alphabet.codes['u'].len == 5);
        TEST(alphabet.codes['x'].len == 5);
    }

    return status;
}
