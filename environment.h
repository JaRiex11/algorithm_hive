#include "point.h"

typedef void* environment;

point iteration(environment env); // итерация алгоритма

void honey_distribution(environment env); // распределение медоносных пчел

void sniffer_distribution(environment env); // распределение разведчиков

void ranging_solutions(environment env); // ранжирование решений

environment create_environment(int sniffer_amount, int honey_amount, int distance, int iterations, const item* items);

void destroy_environment(environment env);