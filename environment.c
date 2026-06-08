#include "environment.h"
#include <stdlib.h>
#include <stdio.h>

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
    int basket_size;
};

int compare_points(const void* a, const void* b) {
    return pnt_quality(*(point*)a) - pnt_quality(*(point*)b);
}

point env_iteration(environment env_) {
    struct environment_* env = env_;
    env_sniffer_distribution(env);
    qsort(env->sniffers_and_honyes, env->sniffer_amount, sizeof(point), compare_points);

    int it = 0;
    while(it++ < env->iterations){
        env_honey_distribution(env);
        env_sniffer_distribution(env);
        ranging_solutions(env);

        env->best_point = env->sniffers[0];
        int best_quality = pnt_quality(env->best_point);
        if (pnt_quality(*env->sniffers) < best_quality) {
            env->best_point = *env->sniffers;
            best_quality = pnt_quality(env->best_point);
        }
    }
    return env->best_point;
}

void env_honey_distribution(environment env_) {
    struct environment_* env = env_;
    for (int i = 0; i < env->honey_amount; i++) {
        if (env->honyes[i])
            destroy_point(env->honyes[i]);
        env->honyes[i] = create_neighbour_point(env->sniffers[rand() % env->sniffer_amount], env->distance);
    }
}

void env_sniffer_distribution(environment env_) {
    struct environment_* env = env_;
    for (int i = 0; i < env->sniffer_amount; i++) {
        if (env->sniffers[i])
            destroy_point(env->sniffers[i]);
        env->sniffers[i] = create_point(env->items, env->basket_size);
    }
}

void ranging_solutions(environment env_) { // ранжирование решений
    struct environment_* env = env_;
    qsort(env->sniffers_and_honyes, env->sniffer_amount + env->honey_amount, sizeof(point), compare_points);
}

environment create_environment(int sniffer_amount, int honey_amount, int distance, int iterations, char* filename) {
    struct environment_* env = malloc(sizeof(struct environment_));
    env->sniffers_and_honyes = calloc((sniffer_amount + honey_amount), sizeof(point));
    env->sniffers = env->sniffers_and_honyes;
    env->honyes = env->sniffers_and_honyes + sniffer_amount;
    env->sniffer_amount = sniffer_amount;
    env->honey_amount = honey_amount;
    env->distance = distance;
    env->iterations = iterations;

    FILE* f = fopen(filename, "r");
    if (!f) {
        perror("input.txt");
        return 1;
    }
    int basket_size, item_count;
    if (fscanf(f, "%d %d",
               &basket_size, &item_count) != 2) {
        fprintf(stderr, "input.txt: expected basket_size sniffer honey distance iterations item_count\n");
        fclose(f);
        return 1;
    }

    item* items = malloc((item_count + 1) * sizeof(item));
    for (int i = 0; i < item_count; i++) {
        if (fscanf(f, "%d", &items[i].volume) != 1) {
            fprintf(stderr, "input.txt: expected %d item volumes\n", item_count);
            free(items);
            fclose(f);
            return 1;
        }
    }
    items[item_count].volume = -1;
    fclose(f);

    env->basket_size = basket_size;
    env->items = items;
    return env;
}

void destroy_environment(environment env_) {
    struct environment_* env = env_;
    for (int i = 0; i < env->sniffer_amount; i++)
        if (env->sniffers[i])
            destroy_point(env->sniffers[i]);
    for (int i = 0; i < env->honey_amount; i++)
        if (env->honyes[i])
            destroy_point(env->honyes[i]);
    free(env->sniffers_and_honyes);
    free(env->items);
    free(env);
}

void env_set_sniffer_amount(environment env_, int sniffer_amount){
    struct environment_* env = env_;
    env->sniffer_amount = sniffer_amount;
    env->sniffers_and_honyes = calloc((sniffer_amount + env->honey_amount), sizeof(point));
    env->honyes = env->sniffers_and_honyes + sniffer_amount;
}
void env_set_honey_amount(environment env_, int honey_amount){
    struct environment_* env = env_;
    env->honey_amount = honey_amount;
    env->sniffers_and_honyes = calloc((env->sniffer_amount + honey_amount), sizeof(point));
}
void env_set_distance(environment env_, int distance){
    struct environment_* env = env_;
    env->distance = distance;
}
void env_set_iterations(environment env_, int iterations){
    struct environment_* env = env_;
    env->iterations = iterations;
}

void env_load_file(environment env_, char* filename){
    struct environment_* env = env_;
    FILE* f = fopen(filename, "r");
    if (!f) {
        perror("input.txt");
        return 1;
    }
    int basket_size, item_count;
    if (fscanf(f, "%d %d",
               &basket_size, &item_count) != 2) {
        fprintf(stderr, "input.txt: expected basket_size sniffer honey distance iterations item_count\n");
        fclose(f);
        return 1;
    }

    item* items = malloc((item_count + 1) * sizeof(item));
    for (int i = 0; i < item_count; i++) {
        if (fscanf(f, "%d", &items[i].volume) != 1) {
            fprintf(stderr, "input.txt: expected %d item volumes\n", item_count);
            free(items);
            fclose(f);
            return 1;
        }
    }
    items[item_count].volume = -1;
    fclose(f);
    
    free(env->items);
    env->items = items;
    env->basket_size = basket_size;
}