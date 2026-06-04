#include "point.h"
#include "item.h"
#include<stdlib.h>
#include<memory.h>

struct point_
{
    item* items;
    int* baskets;
    int baskets_size;
};

#define point struct point_*

int validate(point p) {
    int* baskets_value;
    int max_value = -1;

    for (int i = 0; i < p->baskets_size; i++) { // Нахождение макс значения в масиве корзин
        if (p->baskets[i] > max_value) {        // Что эквивалентно нахождению кол-ва корзин
            max_value = p->baskets[i];
        }
    }

    baskets_value = malloc(max_value * sizeof(int)); // выделение памяти массиву

    memset(baskets_value, 0, max_value * sizeof(int)); // инициализация эл-ов массива нулями

    for (int i = 0; i < p->baskets_size; i++) { // Считаем занятый объем каждой корзины
        baskets_value[p->baskets[i]] += p->items[i].volume;
    }

    for (int i = 0; i < max_value; i++) {
        if (baskets_value[i] <= basket_size) {
            free(baskets_value);
            return 0;
        }
    }

    free(baskets_value);
    return 1;
}

int quality(point p) {
    int n = p->baskets_size;
    int max_b = -1;
    for (int i = 0; i < n; i++)
        if (p->baskets[i] > max_b)
            max_b = p->baskets[i];
    if (max_b < 0)
        return 0;

    int bins = max_b + 1;
    int* load = calloc(bins, sizeof(int));
    for (int i = 0; i < n; i++)
        load[p->baskets[i]] += p->items[i].volume;

    for (int b = 0; b < bins; b++) {
        if (load[b] > basket_size) {
            free(load);
            return 1000000;
        }
    }

    int* vols = malloc(bins * sizeof(int));
    int k = 0;
    for (int b = 0; b < bins; b++)
        if (load[b] > 0)
            vols[k++] = load[b];

    for (int i = 0; i < k; i++)
        for (int j = i + 1; j < k; j++)
            if (vols[j] > vols[i]) {
                int t = vols[i];
                vols[i] = vols[j];
                vols[j] = t;
            }

    int* merged = malloc(k * sizeof(int));
    int merged_count = 0;
    for (int i = 0; i < k; i++) {
        int placed = 0;
        for (int m = 0; m < merged_count; m++) {
            if (merged[m] + vols[i] <= basket_size) {
                merged[m] += vols[i];
                placed = 1;
                break;
            }
        }
        if (!placed)
            merged[merged_count++] = vols[i];
    }

    free(load);
    free(vols);
    free(merged);
    return merged_count;
}

int correct_isomorph(point p) {
    int* buffer;
    buffer = malloc(p->baskets_size * sizeof(int)); // выделение памяти массиву

    memcpy(buffer, p->baskets, p->baskets_size * sizeof(int));
    int cnt = 0;
    for (int i = 0; i < p->baskets_size; i++) {
        int cur_bask = buffer[i];
        if (cur_bask == -1) continue;

        for (int j = i; j < p->baskets_size; j++) {
            if (buffer[j] == cur_bask) {
                p->baskets[j] = cnt;
                buffer[j] = -1;
            }
        }
        cnt++;
    }
}

point create_point(item* items) {
    int n = 0;
    while (items[n].volume != -1)
        n++;

    point p = malloc(sizeof(struct point_));
    p->items = items;
    p->baskets_size = n;
    p->baskets = malloc(n * sizeof(int));

    for (int i = 0; i < n; i++)
        p->baskets[i] = rand() % n;

    return p;
}

point destroy_point(point p) {
    free(p->baskets);
    free(p);
    return 0;
}

point create_neighbour_point(point p, int distance) {
    int n = p->baskets_size;

    point q = malloc(sizeof(struct point_));
    q->items = p->items;
    q->baskets_size = n;
    q->baskets = malloc(n * sizeof(int));
    memcpy(q->baskets, p->baskets, n * sizeof(int));

    if (distance <= 0 || n == 0)
        return q;
    if (distance > n)
        distance = n;

    int* touched = calloc(n, sizeof(int));
    int done = 0;
    while (done < distance) {
        int i = rand() % n;
        if (touched[i])
            continue;
        touched[i] = 1;

        int b = rand() % (n + 1);
        while (b == q->baskets[i])
            b = rand() % (n + 1);
        q->baskets[i] = b;
        done++;
    }
    free(touched);
    return q;
}