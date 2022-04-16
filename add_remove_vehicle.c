/*
 * Adding and removing vehicles 
 * Made by SW1A402 in P1 year 2020
 * 
 * Purpose: Activate, deactivate and pause vehicles from the simulation
 */
#include <stdlib.h>
#include "network_definitions.h"
#include "add_remove_vehicle.h"

/* Sets a vehicle to be active, meaning it is getting simulated */
void activate_vehicle(vehicle *ptr_to_vehicle){
    if (ptr_to_vehicle->cur_activity == inactive || ptr_to_vehicle->cur_activity == active_at_start || ptr_to_vehicle->cur_activity == waiting){
        ptr_to_vehicle->cur_activity = active;
    }
}

/* Sets a vehicle to be inactive, meaning it is not getting simulated */
void deactivate_vehicle(vehicle *ptr_to_vehicle){
    if (ptr_to_vehicle->cur_activity == active || ptr_to_vehicle->cur_activity == active_at_start || ptr_to_vehicle->cur_activity == waiting){
        ptr_to_vehicle->cur_activity = inactive;
    }
}

/* Sets a vehicle to be waiting, meaning it is not getting simulated but signifies it could be reactivated */
void stop_vehicle(vehicle *ptr_to_vehicle){
    if (ptr_to_vehicle->cur_activity == active){
        ptr_to_vehicle->cur_activity = waiting;
    }
}
