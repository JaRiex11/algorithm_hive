#include "ui.h"
#include "easy_plot_c.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "rng.h"


int main(int argc, char **argv) {
    unsigned seed = (unsigned)time(NULL);
    //printf("%d", seed);
    srand(seed);
    //srand(1780996315);

    easy_plot_init(&argc, argv);

    rng_t rng_main;

    rng_seed(&rng_main, seed);

    /*environment env = create_environment(50, 200, 3, 100, "input.txt");
    point result = env_iteration(env);
    print_point(result);

    destroy_environment(env);*/

    run_ui();

    easy_plot_wait();

    return 0;
}
