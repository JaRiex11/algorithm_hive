#ifndef RNG_H
#define RNG_H

#include <stdint.h>

typedef struct rng {
    uint32_t state;
} rng_t;

extern rng_t rng_global;

/* seed == 0 заменяется на 1 (нулевое состояние застревает) */
void rng_seed(rng_t *rng, uint32_t seed);

/* Следующее uint32, верхний бит всегда 0 (для glibc-совместимых диапазонов) */
uint32_t rng_next_u32(rng_t *rng);

/* Равномерно в [0, n), n > 0. При n <= 0 возвращает 0 */
int rng_range(rng_t *rng, int n);

/* Равномерно в [0.0, 1.0) */
double rng_uniform01(rng_t *rng);

#endif
