#ifndef BUS_BEHAVIOR
#define BUS_BEHAVIOR

#include "network_definitions.h"

void printbus(busstop *bus_stop);

void move_bus_to_busstop(busstop *bus_stop, vehicle *bus);
void board_bus(busstop *bus_stop);
double dwell_time(int passengers);

void bus_stop_function(busstop *bus_stop, double delta_time);

void move_bus_out_from_busstop(busstop *bus_stop);

void make_all_buses(network *net);
double random_max_acceleration_bus();

#endif
