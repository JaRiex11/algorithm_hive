#include "rng.h"

void rng_seed(rng_t *rng, uint32_t seed) {
    if (!rng)
        return;
    rng->state = seed ? seed : 1u;
    rng_global.state = seed;
}

rng_t rng_global;

uint32_t rng_next_u32(rng_t *rng) {
    /*uint32_t x;
    #pragma omp critical
{
    x = rng->state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    rng->state = x;
}
    return x >> 1;
*/
    return rand();
}

int rng_range(rng_t *rng, int n) {
    return rand() % n;
    /*if (!rng || n <= 0)
        return 0;

    const uint32_t bound = (uint32_t)n;
    const uint32_t limit = (UINT32_MAX / bound) * bound;

    uint32_t x;
    do {
        x = rng_next_u32(rng);
    } while (x >= limit);

    return (int)(x % bound);*/
}

double rng_uniform01(rng_t *rng) {
    return rng_next_u32(rng) / 2147483648.0; /* 2^31 */
}
