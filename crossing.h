#ifndef CROSSING
#define CROSSING
#include "network_definitions.h"

void update_crossing(vertex *cross, double delta_time);
int in_is_green(vertex *cross, road *in_road);
int out_is_open(vertex *cross, road *out_road);
int index_of_out_road(vertex *vertex, road *road);
int index_of_in_road(vertex *vertex, road *road);

#endif
