#include "item.h"

typedef void* point;

int basket_size;

int validate(point p);

int quality(point p);

int correct_isomorph(point p);

point create_point(item* items);

point destroy_point(point p);

point create_neighbour_point(point p, int distance);