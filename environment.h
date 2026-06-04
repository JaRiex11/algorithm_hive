#include "point.h"

typedef void* environment;

int sniffer_amount; // количество разведчиков (нюкачей)
int honey_amount; // количество медоносных пчел
int best_point_index; // индекс лучшей точки
point best_point; // лучшая точка

int iteration(environment env); // итерация алгоритма

void honey_distribution(environment env); // распределение медоносных пчел

void sniffer_distribution(environment env); // распределение разведчиков

void ranging_solutions(environment env); // ранжирование решений