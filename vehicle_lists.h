#include "network_definitions.h"

#ifndef VEHICLE_LISTS
#define VEHICLE_LISTS


void enqueue_vhcl(road* rd, vehicle *vhcl);
vehicle *dequeue_vhcl(road* rd);
vehicle* get_last_vehicle(road* rd);
vehicle *remove_vhcl_index(road* rd, int index);
void insert_vhcl_index(road* rd, vehicle *vhcl, int index);
int get_vehicle_index(road* rd, vehicle *vhcl);
vehicle* get_vehicle_pointer(road* rd ,int index);

#endif
