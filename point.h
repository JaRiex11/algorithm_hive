#include "item.h"

typedef void* point;

int pnt_validate(point p);

double pnt_quality(point p);

void pnt_correct_isomorph(point p);

point create_point(item* items, int basket_size);

void destroy_point(point p);

point create_neighbour_point(point p, int distance);

point copy_point(point p_);
