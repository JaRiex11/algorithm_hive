#include "point.h"

typedef void* environment;

point env_iteration(environment env); // итерация алгоритма

void env_honey_distribution(environment env); // распределение медоносных пчел

void env_sniffer_distribution(environment env); // распределение разведчиков


environment create_environment(int sniffer_amount, int honey_amount, int distance, int iterations, char* filename) ;

void destroy_environment(environment env);

void env_set_sniffer_amount(environment env, int sniffer_amount);
void env_set_honey_amount(environment env, int honey_amount);
void env_set_distance(environment env, int distance);
void env_set_iterations(environment env, int iterations);
void env_load_file(environment env_, char* filename);