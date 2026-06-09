#include "environment.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

int rnd_linear(int n) {
    if (n <= 1) return 0;
    int total = n * (n + 1) / 2;
    int r = rand() % total;
    int cum = 0;
    for (int i = 0; i < n; i++) {
        cum += n - i;
        if (r < cum) return i;
    }
    return 0;
}

#define LOFFSET log(RAND_MAX)
static double getLrand(double l) {
    return (log(rand() + 1) - LOFFSET) / (-l);
}

int rnd_exp(int n) {
    if (n <= 1) return 0;
    double u = getLrand(2.0);
    int idx = (int)(u * n);
    if (idx >= n) idx = n - 1;
    return idx;
}

int rnd_uniform(int n) {
    if (n <= 0) return 0;
    return rand() % n;
}

int rnd_square(int n) {
    if (n <= 1) return 0;
    int total = 0;
    for (int i = 0; i < n; i++) {
        int w = n - i;
        total += w * w;
    }
    int r = rand() % total;
    int cum = 0;
    for (int i = 0; i < n; i++) {
        int w = n - i;
        cum += w * w;
        if (r < cum) return i;
    }
    return 0;
}

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

    double percent_range;
    int (*function_honey_distribution)(int);
};

int compare_points(const void* a, const void* b) {
    point pa = *(point*)a;
    point pb = *(point*)b;
    if (!pa && !pb) return 0;
    if (!pa) return 1;
    if (!pb) return -1;
    double diff = pnt_quality(pa) - pnt_quality(pb);
    if(diff < 2e-6 && diff > -2e-6) return 0;
    if(diff < 0) return -1;
    return 1;
}

#include <omp.h>

point env_iteration(environment env_) {
    struct environment_* env = env_;
    env_sniffer_distribution(env);
    qsort(env->sniffers_and_honyes, env->sniffer_amount, sizeof(point), compare_points);
    if(env->best_point == 0) env->best_point = copy_point(*env->sniffers);

    double tmp = env->percent_range;
    env->percent_range = 0;
    env_honey_distribution(env);
    env->percent_range = tmp;
    ranging_solutions(env);
        
    printf("\n--------------------------------------------------\n");

    int crat = env->iterations / 50;
    if(crat == 0) crat = 1;

    int it = 0;
    while(it++ < env->iterations){
        if(it % crat == 0) printf("#");
        //printf("iteration no %d\n", it);
        env_honey_distribution(env);
        env_sniffer_distribution(env);
        ranging_solutions(env);

        int best_quality = pnt_quality(env->best_point);
        if (pnt_quality(*env->sniffers) < best_quality) {
            destroy_point(env->best_point);
            env->best_point = copy_point(*env->sniffers);
            best_quality = pnt_quality(env->best_point);
        }
    }
    printf("\n");
    return env->best_point;
}

/*
ga 292
*/

void env_honey_distribution(environment env_) {
    struct environment_* env = env_;
    int count = (env->sniffer_amount + env->honey_amount) * env->percent_range;
    if(count < 1) count = 1;
#pragma omp parallel for
    for (int i = 0; i < env->honey_amount; i++) {
        if (env->honyes[i])
            destroy_point(env->honyes[i]);
        int parent_idx = env->function_honey_distribution(count);
        if (parent_idx < 0)
            parent_idx = 0;
        if (parent_idx >= env->sniffer_amount + env->honey_amount)
            parent_idx = env->sniffer_amount + env->honey_amount - 1;
        point parent = env->sniffers_and_honyes[parent_idx];
        if (!parent && env->sniffer_amount > 0)
            parent = env->sniffers[0];
        if (!parent)
            continue;
        env->honyes[i] = create_neighbour_point(parent, env->distance);
    }
}

void env_sniffer_distribution(environment env_) {
    struct environment_* env = env_;
#pragma omp parallel for
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
    env->percent_range = 0.2;
    env->best_point = 0;
    env->function_honey_distribution = rnd_uniform;

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
    if (env->best_point)
        destroy_point(env->best_point);
    free(env->sniffers_and_honyes);
    free(env->items);
    free(env);
}

static void env_resize_points(struct environment_* env, int new_sniffer, int new_honey) {
    int old_sniffer = env->sniffer_amount;
    int old_honey = env->honey_amount;
    point* old_arr = env->sniffers_and_honyes;

    int new_total = new_sniffer + new_honey;
    point* new_arr = calloc(new_total, sizeof(point));
    if (!new_arr)
        return;

    int sniffer_keep = old_sniffer < new_sniffer ? old_sniffer : new_sniffer;
    for (int i = 0; i < sniffer_keep; i++)
        new_arr[i] = old_arr[i];

    for (int i = new_sniffer; i < old_sniffer; i++)
        if (old_arr[i])
            destroy_point(old_arr[i]);

    int honey_keep = old_honey < new_honey ? old_honey : new_honey;
    for (int i = 0; i < honey_keep; i++)
        new_arr[new_sniffer + i] = old_arr[old_sniffer + i];

    for (int i = new_honey; i < old_honey; i++)
        if (old_arr[old_sniffer + i])
            destroy_point(old_arr[old_sniffer + i]);

    free(old_arr);

    if (env->best_point) {
        destroy_point(env->best_point);
        env->best_point = 0;
    }

    env->sniffer_amount = new_sniffer;
    env->honey_amount = new_honey;
    env->sniffers_and_honyes = new_arr;
    env->sniffers = new_arr;
    env->honyes = new_arr + new_sniffer;
}

void env_set_sniffer_amount(environment env_, int sniffer_amount){
    struct environment_* env = env_;
    if (sniffer_amount < 0)
        sniffer_amount = 0;
    env_resize_points(env, sniffer_amount, env->honey_amount);
}
void env_set_honey_amount(environment env_, int honey_amount){
    struct environment_* env = env_;
    if (honey_amount < 0)
        honey_amount = 0;
    env_resize_points(env, env->sniffer_amount, honey_amount);
}
void env_set_distance(environment env_, int distance){
    struct environment_* env = env_;
    env->distance = distance;
}
void env_set_iterations(environment env_, int iterations){
    struct environment_* env = env_;
    env->iterations = iterations;
}
void env_set_func(environment env_, int func){
    struct environment_* env = env_;
    switch (func)
    {
    case 1:
        env->function_honey_distribution = rnd_uniform;
        break;
    case 2:
        env->function_honey_distribution = rnd_linear;
        break;
    case 3:
        env->function_honey_distribution = rnd_square;
        break;
    case 4:
        env->function_honey_distribution = rnd_exp;
        break;
    default:
        break;
    }
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

void env_set_percent_range(environment env_, double percent_range) {
    struct environment_* env = env_;
    env->percent_range = percent_range;
}

int env_get_sniffer_amount(environment env) {
    return ((struct environment_*)env)->sniffer_amount;
}

int env_get_honey_amount(environment env) {
    return ((struct environment_*)env)->honey_amount;
}

int env_get_distance(environment env) {
    return ((struct environment_*)env)->distance;
}

int env_get_iterations(environment env) {
    return ((struct environment_*)env)->iterations;
}

void env_print_loaded_item(environment env) {
    int bask_size = ((struct environment_*)env)->basket_size;
    item* items = ((struct environment_*)env)->items;
    int min = items[0].volume, max = items[0].volume;
    int it = 0;
    while(items[it].volume != -1){
        if(min > items[it].volume) min = items[it].volume;
        if(max < items[it].volume) max = items[it].volume;
        it++;
    }

    printf("basket size = %d\n items count = %d\n volumes = [%d - %d]\n", bask_size, it, min, max);
}

void env_get_percent_range(environment env) {
    return ((struct environment_*)env)->percent_range;
}
