#include "environment.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void) {
    FILE* f = fopen("input.txt", "r");
    if (!f) {
        perror("input.txt");
        return 1;
    }

    int sniffer_amount, honey_amount, distance, iterations, item_count;
    if (fscanf(f, "%d %d %d %d %d %d",
               &basket_size, &sniffer_amount, &honey_amount,
               &distance, &iterations, &item_count) != 6) {
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

    srand((unsigned)time(NULL));

    environment env = create_environment(sniffer_amount, honey_amount, distance, iterations, items);
    point result = iteration(env);
    print_point(result);

    destroy_environment(env);
    free(items);
    return 0;
}
