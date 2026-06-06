#include "environment.h"
#include <stdlib.h>

struct environment_
{
    point* sniffers_and_honyes;
    point* sniffers;
    point* honyes;

    int sniffer_amount; // количество разведчиков (нюкачей)
    int honey_amount; // количество медоносных пчел
    int best_point_index; // индекс лучшей точки
    point best_point; // лучшая точка
    item* items; // список предметов
    int distance;
    int iterations;
};

#define environment void*

int compare_points(const void* a, const void* b) {
    return quality((point*)a) - quality((point*)b);
}

point iteration(environment env_) {
    struct environment_* env = env_;
    sniffer_distribution(env);
    qsort(env->sniffers_and_honyes, env->sniffer_amount, sizeof(point), compare_points);

    int it = 0;
    while(it++ < env->iterations){
        honey_distribution(env);
        sniffer_distribution(env);
        ranging_solutions(env);

        env->best_point = env->sniffers[0];
        int best_quality = quality(env->best_point);
        if (quality(*env->sniffers) < best_quality) {
            env->best_point = *env->sniffers;
            best_quality = quality(env->best_point);
        }
    }
    return env->best_point;
}

void honey_distribution(environment env_) {
    struct environment_* env = env_;
    for (int i = 0; i < env->honey_amount; i++) 
        env->honyes[i] = create_neighbour_point(env->sniffers[rand() % env->sniffer_amount], env->distance);
}

void sniffer_distribution(environment env_) {
    struct environment_* env = env_;
    for (int i = 0; i < env->sniffer_amount; i++) 
        env->sniffers[i] = create_point(env->items);
}

void ranging_solutions(environment env_) {
    struct environment_* env = env_;
    qsort(env->sniffers_and_honyes, env->sniffer_amount + env->honey_amount, sizeof(point), compare_points);
}

