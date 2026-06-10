#include "point.h"
#include<stdlib.h>
#include<limits.h>
#include<memory.h>

struct point_
{
    const item* items;
    int* baskets;
    int baskets_size;
    int baskets_count;
    int basket_size;
};

int pnt_validate(point p_) {
    struct point_* p = p_;
    int* baskets_value;
    int max_value = -1;

    for (int i = 0; i < p->baskets_size; i++) { // Нахождение макс значения в масиве корзин
        if (p->baskets[i] > max_value) {        // Что эквивалентно нахождению кол-ва корзин
            max_value = p->baskets[i];
        }
    }

    baskets_value = malloc((max_value + 1) * sizeof(int)); // выделение памяти массиву

    memset(baskets_value, 0, (max_value + 1) * sizeof(int)); // инициализация эл-ов массива нулями

    for (int i = 0; i < p->baskets_size; i++) { // Считаем занятый объем каждой корзины
        baskets_value[p->baskets[i]] += p->items[i].volume;
    }

    for (int i = 0; i < max_value; i++) {
        if (baskets_value[i] > p->basket_size) {
            free(baskets_value);
            return 0;
        }
    }

    free(baskets_value);
    return 1;
}

/*double pnt_quality(point p_) {
    struct point_* p = p_;
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
        if (load[b] > p->basket_size) {
            free(load);
            return INT_MAX;
        }
    }

    int k = 0;
    for (int b = 0; b < bins; b++)
        if (load[b] > 0)
            load[k++] = load[b];

    for (int i = 0; i < k; i++)
        for (int j = i + 1; j < k; j++)
            if (load[j] > load[i]) {
                int t = load[i];
                load[i] = load[j];
                load[j] = t;
            }

    int merged_count = 0;
    for (int i = 0; i < k - 1; i++) {
        for (int m = i + 1; m < k; m++) {
            if (load[i] && load[m] && load[m] + load[i] <= p->basket_size) {
                load[m] += load[i];
                load[i] = 0;
            }
        }
    }

    double sum = 0;

    for (int m = 0; m < k; m++){ 
        if(!load[m]) continue;
        merged_count++;
        double x = (double)load[m] / p->basket_size;
        //x = 1. / x;
        sum += x * x * x;
    }

    sum /= merged_count;

    //printf("%d\n", merged_count);
    //printf("%lf\n", sum);

    free(load);
    return merged_count + sum;
}*/

double pnt_quality(point p_) {
    struct point_* p = p_;
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
        if (load[b] > p->basket_size) {
            free(load);
            return INT_MAX;
        }
    }

    int merged_count = 0;

    double sum = 0;

    for (int m = 0; m < bins; m++){ 
        if(!load[m]) continue;
        merged_count++;
        double x = (double)load[m] / p->basket_size;
        //x = 1. / x;
        sum += x * x * x;
    }

    sum /= merged_count;

    //printf("%d\n", merged_count);
    //printf("%lf\n", sum);

    free(load);
    return merged_count + sum;
}

int pnt_quality_print(point p_) {
    struct point_* p = p_;
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
        if (load[b] > p->basket_size) {
            free(load);
            return INT_MAX;
        }
    }

    int k = 0;
    for (int b = 0; b < bins; b++)
        if (load[b] > 0){
            load[k] = load[b];
            for (int j = 0; j < n; j++) if(p->baskets[j] == b) p->baskets[j] = k;
            k++;
        }

    for (int i = 0; i < k; i++)
        for (int j = i + 1; j < k; j++)
            if (load[j] > load[i]) {
                int t = load[i];
                load[i] = load[j];
                load[j] = t;
                for (int h = 0; h < n; h++) if(p->baskets[h] == i) p->baskets[h] = -6;
                for (int h = 0; h < n; h++) if(p->baskets[h] == j) p->baskets[h] = i;
                for (int h = 0; h < n; h++) if(p->baskets[h] == -6) p->baskets[h] = j;
            }

    int merged_count = 0;
    for (int i = 0; i < k - 1; i++) {
        for (int m = i + 1; m < k; m++) {
            if (load[i] && load[m] && load[m] + load[i] <= p->basket_size) {
                load[m] += load[i];
                load[i] = 0;
                for (int j = 0; j < n; j++) if(p->baskets[j] == i) p->baskets[j] = m;
            }
        }
    }

    for (int m = 0; m < k; m++) if(load[m]) merged_count++;

    free(load);
    return merged_count;
}

void pnt_correct_isomorph(point p_) {
    struct point_* p = p_;
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
    free(buffer);
}

point create_point(item* items, int basket_size) {
    int n = 0;
    while (items[n].volume != -1)
        n++;

    struct point_* p = malloc(sizeof(struct point_));
    p->items = items;
    p->baskets_size = n;
    p->basket_size = basket_size;
    p->baskets = malloc(n * sizeof(int));

    int* load = calloc(n, sizeof(int));

    for (int i = 0; i < n; i++)
        p->baskets[i] = i;

    for (int i = 1; i < n; i++){
        int j = rand() % (i + 1);
        int tmp = p->baskets[i];
        p->baskets[i] = p->baskets[j];
        p->baskets[j] = tmp;
    }

    for (int i = 0; i < n; i++)
        load[p->baskets[i]] = p->items[i].volume;

    int it = 0;
    for (int i = 1; i < n; i++) {
        if (load[i] == 0)
            continue;

        if (load[it] + load[i] > p->basket_size) {
            if (it >= n - 1)
                break;
            it++;
            i--;
            continue;
        }

        if (it == i)
            continue;

        load[it] += load[i];
        load[i] = 0;
        for (int j = 0; j < n; j++)
            if (p->baskets[j] == i)
                p->baskets[j] = it;
    }

    int max_basket = 0;
    for (int j = 0; j < n; j++)
        if (p->baskets[j] > max_basket)
            max_basket = p->baskets[j];
    p->baskets_count = max_basket + 1;

    free(load);
    return p;
}

void destroy_point(point p_) {
    struct point_* p = p_;
    free(p->baskets);
    free(p);
}

point create_neighbour_point(point p_, int distance) {
    if (!p_)
        return NULL;
    struct point_* p = p_;
    int n = p->baskets_size;
    if (n <= 0)
        return NULL;

    struct point_* q = malloc(sizeof(struct point_));
    q->items = p->items;
    q->baskets_size = n;
    q->basket_size = p->basket_size;
    q->baskets_count = p->baskets_count;
    q->baskets = malloc(n * sizeof(int));
    memcpy(q->baskets, p->baskets, n * sizeof(int));

    int* touched = calloc(n, sizeof(int));

    if (distance > n)
        distance = n;
    if (distance < 0)
        distance = 0;

    int done = 0;
    while (done < distance) {
        int i = rand() % n;
        if (touched[i])
            continue;
        touched[i] = 1;

        int basket_limit = q->baskets_count;
        if (basket_limit > n)
            basket_limit = n;
        if (basket_limit < 2)
            basket_limit = 2;
        int b = rand() % basket_limit;
        while (b == q->baskets[i])
            b = rand() % basket_limit;
        q->baskets[i] = b;
        done++;
    }
    free(touched);

    int* load = calloc(n, sizeof(int));
    for (int i = 0; i < n; i++) {
        load[q->baskets[i]] += q->items[i].volume;
    }

    int next_basket = q->baskets_count;
    for (int j = 0; j < n; j++)
        if (q->baskets[j] + 1 > next_basket)
            next_basket = q->baskets[j] + 1;

    for (int b = 0; b < n; b++) {
        while (load[b] > q->basket_size) {
            int f = load[b] - q->basket_size;
            int found = -1;
            for (int it = 0; it < n; it++) {
                if (q->baskets[it] != b)
                    continue;
                if (found < 0) {
                    found = it;
                    continue;
                }
                if (f > q->items[found].volume && q->items[it].volume > q->items[found].volume)
                    found = it;
                if (f <= q->items[found].volume && q->items[it].volume < q->items[found].volume)
                    found = it;
            }
            if (found < 0 || next_basket >= n)
                break;

            int buf = q->items[found].volume;
            load[b] -= buf;
            int found_b = 0;
            for(; found_b < next_basket; found_b++){
                if(load[found_b] + buf <= q->basket_size) break;
            }
            load[found_b] += q->items[found].volume;
            q->baskets[found] = found_b;
            if(found_b == next_basket) next_basket++;
        }
    }

    int it = 0;
    for (int i = 1; i < n; i++) {
        if (load[i] == 0)
            continue;

        if (load[it] + load[i] > q->basket_size) {
            if (it >= n - 1)
                break;
            it++;
            i--;
            continue;
        }

        if (it == i)
            continue;

        load[it] += load[i];
        load[i] = 0;
        for (int j = 0; j < n; j++)
            if (q->baskets[j] == i)
                q->baskets[j] = it;
    }

    int max_basket = 0;
    for (int j = 0; j < n; j++)
        if (q->baskets[j] > max_basket)
            max_basket = q->baskets[j];
    q->baskets_count = max_basket + 1;

    free(load);
    
    return q;
}

point copy_point(point p__) {
    struct point_* p_ = p__;
    struct point_* p = malloc(sizeof(struct point_));
    p->items = p_->items;
    p->baskets_size = p_->baskets_size;
    p->basket_size = p_->basket_size;
    p->baskets_count = p_->baskets_count ;
    p->baskets = malloc(p->baskets_size * sizeof(int));
    memcpy(p->baskets, p_->baskets, p->baskets_size * sizeof(int));
    return p;
}

void print_point(point p_) {
    struct point_* p = p_;
    pnt_correct_isomorph(p);

    int n_view = 30;
    int n = p->baskets_size;

    int max_b = -1;
    for (int i = 0; i < n; i++)
        if (p->baskets[i] > max_b)
            max_b = p->baskets[i];
    if (max_b < 0)
        return;
    max_b++;

    int* load = calloc(max_b, sizeof(int));
    int it = 0;
    
    for (int i = 0; i < n; i++)
        load[p->baskets[i]] += p->items[i].volume;

    printf("quality: %d\n", pnt_quality_print(p));

    for (int i = 0; i < max_b; i++)
        load[i] = 0;
    for (int i = 0; i < n; i++)
        load[p->baskets[i]] += p->items[i].volume;

    int itb = 1;
    for (int i = 0; i < max_b; i++){
        if(load[i] == 0) continue;
        printf("basket %3d [", itb);
        int m_view = n_view * load[i] / p->basket_size;
        if(m_view == 0) m_view = 1;
        it = 0;
        for(;it < m_view; it++) printf("#");
        for(;it < n_view; it++) printf(" ");
        printf("] %d / %d ( ", load[i], p->basket_size);
        int f_space = 0;
        for(it = 0; it < n; it++) if(p->baskets[it] == i){
            if(f_space) printf(" ");
            else f_space = 1;
            printf("%d", p->items[it].volume);
        }
        printf(" )\n");
        itb++;
    }

    free(load);
}