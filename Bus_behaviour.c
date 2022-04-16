/*
 * Bus behavior models and implementation 
 * Made by SW1A402 in P1 year 2020
 * 
 * Purpose: Holds function related to buses, busstops and the interaction between them
 *              Moving a bus into and out of a busstop 
 *              Updating busstops
 *              Making all the buses
 */

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include "Bus_behaviour.h"
#include "traffic_sim.h"
#include "add_remove_vehicle.h"
#include "vehicle_lists.h"
#include "vehicle_behavior.h"
#include "Traffic_sim_manager.h"

#define BOARDING_TIME 2.3
#define DOOR_TIME 6

#define MIN_MAX_ACCELERATION_BUS 0.9
#define MAX_MAX_ACCELERATION_BUS 1.1
#define BUS_LENGTH 13.5

/* Run ONLY move_bus_to_busstop if you want to move a bus to busstop */
void move_bus_to_busstop(busstop *bus_stop, vehicle *bus){
    int index;
    stop_vehicle(bus);
    index = get_vehicle_index(bus->cur_road, bus);
    bus_stop->bus = remove_vhcl_index(bus->cur_road, index);
    bus_stop->a_bus_at_the_stop = 1;
    board_bus(bus_stop);
}

/* Board all passengers at the stop, and get the dwell time accordingly */
void board_bus(busstop *bus_stop){
    bus_stop->cur_waiting_time = dwell_time(bus_stop->cur_waiting);
    bus_stop->cur_waiting = 0;
}

/* Calculates and returns dwell time */
double dwell_time(int passengers){
    return passengers * BOARDING_TIME + DOOR_TIME;
}

/* Run bus_stop_function every cycle */
void bus_stop_function(busstop *bus_stop, double delta_time){
    if (bus_stop->a_bus_at_the_stop){
        bus_stop->cur_waiting_time -= delta_time;
        if (bus_stop->cur_waiting_time <= 0)
            move_bus_out_from_busstop(bus_stop);
        else if (bus_stop->cur_waiting_time <= 1)
            bus_stop->bus_coming_out_bool = 1;
    }
    bus_stop->cur_waiting += bus_stop->people_per_sec * delta_time;
}

/* Run ONLY move_bus_out_from_busstop if you want to move a bus out from busstop to next road */
void move_bus_out_from_busstop(busstop *bus_stop){
    road *next_road;
    route route_bus = bus_stop->bus->cur_route;
    activate_vehicle(bus_stop->bus);
    next_road = road_from_to(route_bus.points[route_bus.cur_point], route_bus.points[route_bus.cur_point + 1], road_bus);
    enqueue_vhcl(next_road, bus_stop->bus);

    bus_stop->bus_coming_out_bool = 0;
    bus_stop->a_bus_at_the_stop = 0;
}

/* Makes all bus vehicles with random values */
void make_all_buses(network *net){
    int i;
    double bus_length = ((BRT == 1) ? 25 : 13.5);
    for (i = 0; i < net->bus_count; i++)
    {
        net->buses[i] = make_bus(vhcl_bus,
                                     random_max_acceleration_bus(),
                                     random_desired_time_headway(),
                                     0,
                                     bus_length);
    }
}

/* Random acceleration for buses */
double random_max_acceleration_bus()
{
    return random_number_in_interval(MIN_MAX_ACCELERATION_BUS, MAX_MAX_ACCELERATION_BUS);
}
