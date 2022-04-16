/*
 * Vehicle spawner
 * Made by SW1A402 in P1 year 2020
 * 
 * Purpose: Functions relating to spawning vehicles, and managing when to spawn vehicles
 *              spawn_vehicle function for spawning an unused vehicle into the simulation
 *              timed_vehicle_spawner for spawning vehicles if it is time to do so
 *              timed_bus_spawner for spawning buses at their routes if it is time to do so
 */

#include <stdio.h>
#include "network_definitions.h"
#include "spawn_cars.h"
#include "add_remove_vehicle.h"
#include "traffic_sim.h"
#include <assert.h>



/* Spawns cars if it is time to do so */
void timed_vehicle_spawner(network *rd_network, double sim_time, double spawn_time){
    int desired_car;
    static int current_car = 0;
    road *spawn_road;
    vehicle *sp_vehicle;

    /* Get the amount of cars that should be spawned */
    desired_car = sim_time * (rd_network->vehicle_count / spawn_time);

    /* If less cars are spawned than should have been, spawn one */
    if (desired_car > current_car && current_car < rd_network->vehicle_count)
    {
        sp_vehicle = rd_network->vehicles + current_car;
        spawn_road = road_from_to(sp_vehicle->cur_route.points[0],sp_vehicle->cur_route.points[1], road_any);
        spawn_vehicle(sp_vehicle, spawn_road, sim_time);
        current_car++;
        if (TRACE)printf("Spawned car");
    }
}

/* Spawns a single vehicle to a given road */
void spawn_vehicle(vehicle*ptr_to_vehicle, road *spawn_road, double cur_time){
    enqueue_vhcl(spawn_road, ptr_to_vehicle);
    activate_vehicle(ptr_to_vehicle);
    ptr_to_vehicle->spawn_time = cur_time;
}


/* ----------------------------- Start: Spawn function for buses ------------------------------------- */

/* Checks all busroutes to spawn a bus if necessary */
void timed_bus_spawner(network *net, double sim_time, double delta_time){
    int i = 0;
    for (i = 0; i < net->busroute_count; i++)
    {
        check_busroute(i, net, sim_time, delta_time);
    }
}

/* Checks whether a bus should be spawned, and counts down busspawn timer */
void check_busroute(int i, network *net, double sim_time, double delta_time){
    busroute *cur_busroute = &(net->busroutes[i]);
    static int current_bus = 0;
    vehicle *spawn_bus;
    road *start_road_spawn_bus;
    
    if(cur_busroute->timer > 0){
        cur_busroute->timer -= delta_time;
    } 
    else{ 
        if (TRACE) printf("spawn_bus current_bus: %d\n",current_bus);
        spawn_bus = net->buses + current_bus;
        start_road_spawn_bus = road_from_to(cur_busroute->b_route.points[0], cur_busroute->b_route.points[1], road_bus);
        spawn_bus->cur_route = cur_busroute->b_route;
        spawn_bus->cur_route.bus_id = i;
        spawn_vehicle(spawn_bus, start_road_spawn_bus, sim_time);
        cur_busroute->timer = cur_busroute->bus_interval;
        current_bus++;
    }    
}

/* ----------------------------- End:   Spawn function for buses ------------------------------------- */
