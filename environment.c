#include "environment.h"
#include <stdlib.h>

struct environment_
{
    point* sniffers;
    point* honyes;
};

#define environment struct environment_*

int iteration(environment env) {
    sniffer_distribution(env);
    ranging_solutions(env);
    honey_distribution(env);

    best_point_index = 0;
    best_point = env->sniffers[0];
    for (int i = 0; i < sniffer_amount; i++) {
        if (quality(env->sniffers[i]) < quality(best_point)) {
            best_point_index = i;
            best_point = env->sniffers[i];
        }
    }
    return 0;
}

void honey_distribution(environment env) {
    for (int i = 0; i < honey_amount; i++) {
        env->honyes[i] = create_neighbour_point(env->sniffers[rand() % sniffer_amount], rand() % 10);
    }
}

void sniffer_distribution(environment env) {
    for (int i = 0; i < sniffer_amount; i++) {
        env->sniffers[i] = create_point(items);
    }
}

void ranging_solutions(environment env) {
    qsort(env->sniffers, sniffer_amount, sizeof(point), compare_points);
}

int compare_points(const void* a, const void* b) {
    return quality((point*)a) - quality((point*)b);
}
