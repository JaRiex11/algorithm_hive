#include "ui.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


int main(void) {
    unsigned seed = (unsigned)time(NULL);
    //printf("%d", seed);
    srand(seed);
    //srand(1780996315);

    /*environment env = create_environment(50, 200, 3, 100, "input.txt");
    point result = env_iteration(env);
    print_point(result);

    destroy_environment(env);*/

    run_ui();

    return 0;
}
