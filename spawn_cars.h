#ifndef SPAWN_CARS
#define SPAWN_CARS
#include "network_definitions.h"
#include "vehicle_lists.h"
#include "vehicle_behavior.h"


void timed_vehicle_spawner(network *rd_network, double sim_time, double spawn_time);
void spawn_vehicle(vehicle *ptr_to_vehicle, road *test_road, double cur_time);

void timed_bus_spawner(network *net, double sim_time, double delta_time);
void check_busroute(int i, network *net, double sim_time, double delta_time);

#endif
