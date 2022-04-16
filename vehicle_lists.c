/*
 * Functions for handling the vehicles on a road as a linked list
 * Made by SW1A402 in P1 year 2020
 * 
 * Purpose: Functions for adding, removing and accessing vehicles in the
 *              linked list of vehicles that roads contain.
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "network_definitions.h"
#include "vehicle_lists.h"
#include "vehicle_behavior.h"
#include "traffic_sim.h"

/* Inserts vehicle vhcl as the first element (furthest back) in a road */
void enqueue_vhcl(road* rd, vehicle* vhcl){
    insert_vhcl_index(rd, vhcl, 0);
    vhcl->position = 0;
}

/* Removes and returns the last element (front vehicle pointer) from a roads (rd) list of vehicles  */
vehicle *dequeue_vhcl(road* rd){
    vehicle *vhcl_last = remove_vhcl_index(rd, rd->vhcl_count - 1);

    assert(vhcl_last != NULL);
    return vhcl_last;
}

/* Inserts vehicle vhcl at index "index" in the road array, pushing the last elements forward */
void insert_vhcl_index(road* rd, vehicle *vhcl, int index){
    vehicle *vhcl_i;
    if (index == 0){
        vhcl->front_vehicle = rd->last_in_vhcl;
        rd->last_in_vhcl = vhcl;
    }
    else{
        vhcl_i = get_vehicle_pointer(rd, index - 1);
        vhcl->front_vehicle = vhcl_i->front_vehicle;
        vhcl_i->front_vehicle = vhcl;
    }
    vhcl->cur_road = rd;
    rd->vhcl_count++;
}

/* Remove and return vehicle  at the index in road rd */
vehicle *remove_vhcl_index(road* rd, int index){
    vehicle *vhcl_i,
            *vhcl_behind;
    if (index == 0){
        vhcl_i = get_vehicle_pointer(rd, index);
        rd->last_in_vhcl = vhcl_i->front_vehicle;
        vhcl_i->front_vehicle = NULL;
    }
    else{
        vhcl_behind = get_vehicle_pointer(rd, index - 1);
        vhcl_i = vhcl_behind->front_vehicle;
        vhcl_behind->front_vehicle = vhcl_i->front_vehicle;
        vhcl_i->front_vehicle = NULL;
    }
    rd->vhcl_count--;
    vhcl_i->cur_road = NULL;
    assert(vhcl_i != NULL); /* Make sure it exists, else something went wrong at the caller. */
    return vhcl_i;
}

/* Gets pointer to front vehicle of the road and returns it. Doesnt remove it.
    Assumes there exists cars on the road */
vehicle* get_last_vehicle(road* rd){
    vehicle *vhcl = get_vehicle_pointer(rd, rd->vhcl_count - 1);
    assert(vhcl != NULL); /* There are no cars in the road, so something went wrong at the caller or get_road_vehicle */
    return vhcl;
}


/* Get index for vehicle with pointer vhcl in road "rd" without removing it
   MUST be the same pointer that is used in the road linked list */
int get_vehicle_index(road* rd, vehicle *vhcl){
    int i, found_index = -1;
    vehicle *cur_vhcl = rd->last_in_vhcl;
    for (i = 0; i < rd->vhcl_count; i++)
    {
        if ((int)cur_vhcl == (int)vhcl){
            found_index = i;
            break;
        }
        cur_vhcl = cur_vhcl->front_vehicle;
    }
    assert(found_index != -1);
    return found_index;
}

/* Get pointer to vehicle with index "index" in road "rd" without removing it */
vehicle* get_vehicle_pointer(road* rd ,int index){
    int i;
    vehicle *vhcl = rd->last_in_vhcl;
    for (i = 0; i < index; i++)
    {
        if (vhcl != NULL){
            vhcl = vhcl->front_vehicle;
        }
        else{
            break;
        }
    }
    return vhcl;
}
