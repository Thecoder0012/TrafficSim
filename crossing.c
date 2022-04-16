/*
 * Crossing model and implementation 
 * Made by SW1A402 in P1 year 2020
 * 
 * Purpose: Holds function related to crossings, and helper functions to access information from them
 *              Updating crossings
 *              checking if specific roads are open/closed in the crossing
 *              Helper functions to get index of road in crossing from pointer
 */

#include "network_definitions.h"
#include "stdio.h"
#include "traffic_sim.h"
#include "vehicle_behavior.h"
#include "assert.h"
#include "crossing.h"

/* Runs all code for updating a specific crossing. Input vertex MUST be of crossing type. */
void update_crossing(vertex *crossing_vertex, double delta){
    int i;
    crossing *cross = &crossing_vertex->type.cross;
    /* Unblock blocked roads */
    for (i = 0; i < crossing_vertex->roads_out_count; i++)
    {
        cross->out_road_open[i] = 1;
    }
    /* Update phase */
    if (cross->cur_timer < 0){
        cross->current_phase = (cross->current_phase + 1) % cross->phasecount;
        cross->cur_timer += cross->phase_times[cross->current_phase]; 
    }
    cross->cur_timer -= delta;
}

/* Returns whether a light is currently green for the given in road. Must be a crossing */
int in_is_green(vertex *cross, road *in_road){
    return cross->type.cross.current_phase == cross->type.cross.in_roads_phases[index_of_in_road(cross, in_road)];
}

/* Returns whether an out road is currently blocked for the given out road. Must be a crossing */
int out_is_open(vertex *cross, road *out_road){
    return cross->type.cross.out_road_open[index_of_out_road(cross, out_road)];
}

/* Gets the index of an out road for a specific vertex 
    The given road must exist as an out road for the vertex*/
int index_of_out_road(vertex *vertex, road *road){
    int i, index = -1;
    for (i = 0; i < vertex->roads_out_count; i++)
    {
        if (road == vertex->roads_out[i]){
            index = i;
        }
    }
    assert(index > -1); /* Make sure a road was found */
    return index;
}

/* Gets the index of an in road for a specific vertex 
    The given road must exist as an out road for the vertex*/
int index_of_in_road(vertex *vertex, road *road){
    int i, index = -1;
    for (i = 0; i < vertex->roads_in_count; i++)
    {
        if (road == vertex->roads_in[i]){
            index = i;
        }
    }
    assert(index > -1); /* Make sure a road was found */
    return index;
}
