#include "cic_filter.h"
#include <math.h>

#define COMB(history, tmp, input) (tmp = input - history, history = input, tmp)

void downsample32_init(downsample32_state_t *state)
{
    *state = (downsample32_state_t){};
}

// Performs CIC^5 filtering to subsample data 3x.
// Input is N times 16-bit samples, output is N/3 times 32-bit samples.
// The function returns the number of output samples, which can vary if N is not dividable by 3.
// This uses 32-bit arithmetic and is faster as an initial step than analog_downsampleX().
// This function gives a gain of 6**5 = 7776 (13 bits)
size_t downsample32(downsample32_state_t *state, const uint16_t *input, size_t count, uint32_t *output)
{
    int phase = state->phase;
    size_t in_index = 0;
    size_t out_index = 0;

    while (in_index < count)
    {
        // This first loop handles start and end of buffer one sample at a time
        while (in_index < count && (phase != 0 || in_index + 6 > count))
        {
            state->integrators[0] += input[in_index++];
            state->integrators[1] += state->integrators[0];
            state->integrators[2] += state->integrators[1];
            state->integrators[3] += state->integrators[2];
            state->integrators[4] += state->integrators[3];
            phase += 1;

            if (phase == 3 || phase == 6)
            {
                // At phase 3, update history at index 0.
                // At phase 6, update history at index 1 and reset phase to 0.
                int p = (phase == 6) ? 1 : 0;
                if (p) phase = 0;

                uint32_t tmp[5];
                output[out_index++] = COMB(state->combs[4][p], tmp[4],
                                      COMB(state->combs[3][p], tmp[3],
                                      COMB(state->combs[2][p], tmp[2],
                                      COMB(state->combs[1][p], tmp[1],
                                      COMB(state->combs[0][p], tmp[0],
                                           state->integrators[4])))));
            }
        }

        // This second loop handles middle of buffer 6 samples at a time.
        while (in_index + 6 <= count)
        {
            uint32_t w0 = *(uint32_t*)&input[in_index];
            uint32_t w1 = *(uint32_t*)&input[in_index + 2];
            uint32_t w2 = *(uint32_t*)&input[in_index + 4];
            in_index += 6;

            // Label the input samples s0...s5, integrators i0..i4, combs c00..c40 and c01..c41
            //
            // For the integrator update, we want to perform:
            //    i0 += s0; i1 += i0; i2 += i1; i3 += i2; i4 += i3;
            //    i0 += s1; i1 += i0; i2 += i1; i3 += i2; i4 += i3;
            //    i0 += s2; i1 += i0; i2 += i1; i3 += i2; i4 += i3;
            // This can be optimized to make just one summation per integrator:
            uint16_t s0 = w0 & 0xFFFF;
            uint16_t s1 = w0 >> 16;
            uint16_t s2 = w1 & 0xFFFF;
            state->integrators[4] += state->integrators[3] * 3 + s0 * 15 + s1 * 5 + s2;
            state->integrators[3] += state->integrators[2] * 3 + s0 * 10 + s1 * 4 + s2;
            state->integrators[2] += state->integrators[1] * 3 + s0 *  6 + s1 * 3 + s2;
            state->integrators[1] += state->integrators[0] * 3 + s0 *  3 + s1 * 2 + s2;
            state->integrators[0] += s0 + s1 + s2;

            // Perform the first comb stage
            uint32_t tmp[5];
            output[out_index++] = COMB(state->combs[4][0], tmp[4],
                                  COMB(state->combs[3][0], tmp[3],
                                  COMB(state->combs[2][0], tmp[2],
                                  COMB(state->combs[1][0], tmp[1],
                                  COMB(state->combs[0][0], tmp[0],
                                       state->integrators[4])))));
            
            // Then the same for the second set of 3 samples
            uint16_t s3 = w1 >> 16;
            uint16_t s4 = w2 & 0xFFFF;
            uint16_t s5 = w2 >> 16;
            state->integrators[4] += state->integrators[3] * 3 + s3 * 15 + s4 * 5 + s5;
            state->integrators[3] += state->integrators[2] * 3 + s3 * 10 + s4 * 4 + s5;
            state->integrators[2] += state->integrators[1] * 3 + s3 *  6 + s4 * 3 + s5;
            state->integrators[1] += state->integrators[0] * 3 + s3 *  3 + s4 * 2 + s5;
            state->integrators[0] += s3 + s4 + s5;

            output[out_index++] = COMB(state->combs[4][1], tmp[4],
                                  COMB(state->combs[3][1], tmp[3],
                                  COMB(state->combs[2][1], tmp[2],
                                  COMB(state->combs[1][1], tmp[1],
                                  COMB(state->combs[0][1], tmp[0],
                                  state->integrators[4])))));
        }
    }

    state->phase = phase;
    return out_index;
}

void downsample64_init(downsample64_state_t *state, int divider, int shift)
{
    *state = (downsample64_state_t){};
    state->divider = divider;
    state->shift = shift;

    int d = divider;
    state->gain = (d*d*d*d*d);
}

// Performs CIC^5 filtering to subsample data by arbitrary factor.
// Input is N times 32-bit samples, output is N/d times 32-bit samples.
// The function causes a gain of (2*divider) ** 5.
// The shift of the output values has to be adjusted to keep output data in uint32_t range.
// To avoid overflows, maximum divider in single stage is 32 for 32-bit input data, or 64 for 24-bit input data.
size_t downsample64(downsample64_state_t *state, const uint32_t *input, size_t count, uint32_t *output)
{
    int phase = state->phase;
    int divider = state->divider;
    int shift = state->shift;
    size_t in_index = 0;
    size_t out_index = 0;

    while (in_index < count)
    {
        state->integrators[0] += input[in_index++];
        state->integrators[1] += state->integrators[0];
        state->integrators[2] += state->integrators[1];
        state->integrators[3] += state->integrators[2];
        state->integrators[4] += state->integrators[3];
        phase += 1;

        if (phase == divider || phase == divider * 2)
        {
            // At phase d, update history at index 0.
            // At phase 2d, update history at index 1 and reset phase to 0.
            int p = (phase == divider * 2) ? 1 : 0;
            if (p) phase = 0;

            uint64_t tmp[5];
            uint64_t result = COMB(state->combs[4][p], tmp[4],
                              COMB(state->combs[3][p], tmp[3],
                              COMB(state->combs[2][p], tmp[2],
                              COMB(state->combs[1][p], tmp[1],
                              COMB(state->combs[0][p], tmp[0], state->integrators[4])))));
            
            // Result from comb has gain of (2 * divider)**5
            result >>= shift;
            if (result > 0xFFFFFFFF) result = 0xFFFFFFFF;
            output[out_index++] = result;
        }
    }

    state->phase = phase;
    return out_index;
}

static void choose_factors(int factors[4], int divider)
{
    for (int stage = 0; stage < 4; stage++)
    {
        int max_divider = (stage == 0) ? 32 : 64;
        if (divider <= max_divider)
        {
            // Division can be done in single stage
            factors[stage] = divider;
        }
        else
        {
            // Find exact division factor, if any exists
            for (int d = max_divider; d > 1; d--)
            {
                if (divider % d == 0)
                {
                    factors[stage] = d;
                    break;
                }
            }

            if (factors[stage] == 1)
            {
                // Fallback, will not result in exact ratio
                factors[stage] = max_divider;
            }
        }

        divider /= factors[stage];
    }
}

void downsampleX_init(downsampleX_state_t *state, int divider, uint32_t input_range)
{
    const int target_range = (1 << 24);
    int factors[4];
    choose_factors(factors, divider);

    state->stages = 0;
    state->divider = 1;
    state->gain = 1.0f;

    for (int i = 0; i < 4; i++)
    {
        if (factors[i] > 1)
        {
            int d = factors[i];
            uint64_t output_range = (d*d*d*d*d) * (uint64_t)input_range;
            int shift = 0;
            while (output_range > target_range)
            {
                shift++;
                output_range >>= 1;
            }

            downsample64_state_t *stage = &state->downsamplers[state->stages];
            downsample64_init(stage, factors[i], shift);
            state->gain *= stage->gain / (float)(1 << stage->shift);
            state->divider *= stage->divider;
            state->stages++;
        }
    }
}

size_t downsampleX(downsampleX_state_t *state, const uint32_t *input, size_t count, uint32_t *output)
{
    count = downsample64(&state->downsamplers[0], input, count, output);

    for (int i = 1; i < state->stages; i++)
    {
        count = downsample64(&state->downsamplers[i], output, count, output);
    }

    return count;
}
