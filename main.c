#include "environment.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void) {
    srand((unsigned)time(NULL));

    environment env = create_environment(20, 10, 3, 100, "input.txt");
    point result = env_iteration(env);
    print_point(result);

    destroy_environment(env);
    return 0;
}
