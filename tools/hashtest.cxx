// For quickly hashing words in the deflate algorithm, the code uses a simple
// multiplication by 32 bit constant, followed by discarding the bottom 32 bits.
// This program tries to find optimal constant for the multiplication.

#include <stdio.h>
#include <random>
#include <algorithm>

#define TABLE_SIZE 1024

// Evaluate the constant with random words, return collision count.
int evaluate(uint32_t constant, uint32_t seed)
{
    std::mt19937_64 mt(seed);
    std::uniform_int_distribution<uint32_t> dist(0, 0xFFFFFFFF);
    std::uniform_int_distribution<uint32_t> bitchanges(1, 8);
    std::uniform_int_distribution<uint32_t> bitpos(0, 31);
    int collisions = 0;
    
    for (int i = 0; i < 1000000; i++)
    {
        uint32_t word1 = dist(mt);
        uint32_t word2 = word1;
        uint32_t flipped_bits = bitchanges(mt);
        
        for (int j = 0; j < flipped_bits; j++)
        {
            word2 ^= (1 << bitpos(mt));
        }
        
        uint32_t hashkey1 = ((((uint64_t)word1 * constant) >> 32) + word1) % TABLE_SIZE;
        uint32_t hashkey2 = ((((uint64_t)word2 * constant) >> 32) + word2) % TABLE_SIZE;
        
        if (hashkey1 == hashkey2)
            collisions++;
    }
    
    return collisions;
}

int main(int argc, const char **argv)
{
    std::random_device rd;
    std::mt19937_64 mt(rd());
    std::uniform_int_distribution<uint32_t> dist(0, 0xFFFFFFFF);
    std::uniform_int_distribution<uint32_t> bitchanges(1, 6);
    std::uniform_int_distribution<uint32_t> bitpos(0, 31);
    int best = std::numeric_limits<int>::max();
    uint32_t best_c = 0;
    uint32_t seed = rd();

    printf("Seed: 0x%08x\n", seed);
        
    if (argc > 1)
    {
        best_c = strtoul(argv[1], NULL, 0);
        best = evaluate(best_c, seed);
        printf("Initial: 0x%08x, collisions: %d\n", best_c, best);
    }
    
    for (int i = 0; ; i++)
    {
        if (i > 1000)
        {
            seed = rd();
            best = evaluate(best_c, seed);
            printf("Reseeded: 0x%08x, collisions %d\n", seed, best);
            i = 0;
        }
    
        uint32_t trial;
        
        if (i & 1)
        {
            // Try totally new value
            trial = dist(mt);
        }
        else
        {
            // Modify previous best
            trial = best_c;
            uint32_t flipped_bits = bitchanges(mt);
            for (int j = 0; j < flipped_bits; j++)
            {
                trial ^= (1 << bitpos(mt));
            }
        }
        
        int collisions = evaluate(trial, seed);
        
        if (collisions < best)
        {
            best = collisions;
            best_c = trial;
            printf("Constant: 0x%08x, collisions: %d\n", trial, collisions);
        }
    }
    
    return 0; 
}
