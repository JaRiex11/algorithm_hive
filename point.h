#include "item.h"

typedef void* point;

extern int basket_size;

int validate(point p);

int quality(point p);

void correct_isomorph(point p);

point create_point(item* items);

void destroy_point(point p);

point create_neighbour_point(point p, int distance);

point copy_point(point p_);
