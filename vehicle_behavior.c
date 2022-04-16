/*
 * Contains all the general vehicle models, and their helper functions
 * Made by SW1A402 in P1 year 2020
 * 
 * Purpose: All general vehicle behavior is handled here, and the helper functions necessary for it.
 *              Removing a vehicle from the network
 *              Functions relating to finding and checking specific roads
 *              Functions for finding and setting vehicles current goal
 *              Functions for accelerating a vehicle according to its goal
 *              Functions for moving a vehicle along its route according to speed
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "network_definitions.h"
#include "vehicle_behavior.h"
#include "Bus_behaviour.h"
#include "vehicle_lists.h"
#include "add_remove_vehicle.h"
#include "traffic_sim.h"
#include "crossing.h"

#include "assert.h"
#include "framework.h"

/* Removes a vehicle from the simulation */
void remove_vehicle(vehicle *vhcl, double cur_time){
    remove_vhcl_index(vhcl->cur_road, get_vehicle_index(vhcl->cur_road, vhcl));
    deactivate_vehicle(vhcl);
    if (vhcl->type == vhcl_car){
        sim_log_data(data_car_travel_time, cur_time - vhcl->spawn_time);
    }
    else{
        if (vhcl->cur_route.bus_id == 1){
            sim_log_data(data_bus_north_travel_time, cur_time - vhcl->spawn_time);
            sim_log_data(data_bus_north_headway, cur_time);
        }
        else{
            sim_log_data(data_bus_south_travel_time, cur_time - vhcl->spawn_time);
            sim_log_data(data_bus_south_headway, cur_time);
        }
    }
}

/* Gets the most fitting road between two points, 
    prioritizes roads of type type, otherwise chooses road_any */
road *road_from_to(vertex *v1, vertex *v2, road_type type){
    int i;
    road *cur_road, *chosen_road = NULL;
    for (i = 0; i < v1->roads_out_count; i++)
    {
        cur_road = v1->roads_out[i];
        /* If this road reaches destination and matches type,  */
        if (cur_road->destination == v2){
            /* Choose road if is anyroad and no road has been found, or if it matches type */
            if ((chosen_road == NULL && cur_road->type == road_any) || cur_road->type == type){
                chosen_road = cur_road;
            }
        }
    }
    return chosen_road;
}

/* 1 if the route is at the last part, 0 otherwise */
int at_end_of_route(route *rte){
    return (rte->cur_point + 1) >= (rte->route_length - 1);
}

/* See if it is possible to use an out road of a vertex, if it can, block it temporarily */
int try_use_out_road(vertex *vert, road *rd){
    int index = 0;
    if (vert->vertex_type == v_crossing){
        if (TRACE) printf("Attempting to use crossing, out road is %d", vert->type.cross.out_road_open[index]);
        index = index_of_out_road(vert, rd);
        if (vert->type.cross.out_road_open[index]){
            vert->type.cross.out_road_open[index] = 0;
            return 1;
        }
        else{
            return 0;
        }   
    }
    else{
        return 1;
    }    
}

/* Reset the blocks for the out roads for a vertex */
void reset_out_road(vertex *vert){
    int i = 0;
    if (vert->vertex_type == v_crossing){
        for (i = 0; i < vert->roads_out_count; i++)
        {
            vert->type.cross.out_road_open[i] = 1;
        }
    }
}

/* Move a vehicle from one road to another */
void vehicle_move_road(vehicle *vhcl, road *next_road){
    if (TRACE) printf("moveroad\n");
    if (vhcl->front_vehicle == NULL){
        dequeue_vhcl(vhcl->cur_road);
    }
    else{ /* Handle edge case where 2 vehicles move out of the road simultaneusly, back car isnt the frontmost car*/
        if (TRACE) printf("edgecase\n");
        remove_vhcl_index(vhcl->cur_road, get_vehicle_index(vhcl->cur_road, vhcl));
    }
    if (TRACE) printf("enqueue\n");
    enqueue_vhcl(next_road, vhcl);
    vhcl->position = 0;
}

/* Get the next road for the current route */
road *get_next_road(vehicle *vhcl){
    if (!at_end_of_route(&vhcl->cur_route)){
        return road_from_to(vhcl->cur_route.points[vhcl->cur_route.cur_point + 1], vhcl->cur_route.points[vhcl->cur_route.cur_point + 2], vhcl->type == vhcl_bus ? road_bus : road_any);
    }
    else{
        return NULL;
    }
}

/* Moves a vehicle with its current speed for delta time seconds along its route 
   Mostly just moves along road, but also handles edgecases for:
    Reaching end of route: removes it
    Reaches crossing: moves to next road, unless this is illegal */
void move_vehicle(vehicle *vhcl, double delta_time, double cur_time){
    double movement = vhcl->speed * delta_time;                     
    double road_left = vhcl->cur_road->length - vhcl->position;     
    double excess_move = movement - road_left;                      
    double time_left = (excess_move/movement) * delta_time;         
    road *next_road = NULL;
    route *rte = &(vhcl->cur_route);
    if (TRACE)  printf("Started movement; ");
    if (!(time_left > 0)){ /* Simply move the car along road, as the move doesnt exceed the road */
        if (TRACE) printf("Simple move, no exceed.\n");
        vhcl->position += movement;
    }
    else if(at_end_of_route(rte)){ /* Remove the car, as it reached end of route in this case */
        if (TRACE) printf("End of route. Deleted.\n");
        remove_vehicle(vhcl, cur_time);
    }
    else if(vhcl->type == vhcl_bus && rte->points[rte->cur_point + 1]->vertex_type == v_busstop && rte->points[rte->cur_point + 1]->type.stop.a_bus_at_the_stop == 0){
        if (TRACE) printf("Stopped at busstop.\n");
        move_bus_to_busstop(&(rte->points[rte->cur_point + 1]->type.stop), vhcl);
        rte->cur_point++;
    }
    else{ /* Car is moving past this road, move it along to next road */
        if (TRACE) printf("Must move to new road; ");
        next_road = road_from_to(rte->points[rte->cur_point + 1], rte->points[rte->cur_point + 2], vhcl->type == vhcl_bus ? road_bus : road_any);
        if (try_use_out_road(vhcl->cur_road->destination, next_road))
        {
            vehicle_move_road(vhcl, next_road); 
            rte->cur_point++;
            move_vehicle(vhcl, time_left, cur_time); /* Recursive call to move along at new position and road */
        }
        else{ /* Cannot go out through this road! Panic stop the car */
            if (TRACE) printf("panic stop to avoid collisions.\n");
            vhcl->position = vhcl->cur_road->length;
            vhcl->speed = 0;
        }
    }
}
    
/* Accelerate vehicle vehicle */
void new_speed(vehicle *vehicle, double delta_time){
    double acceleration = calulate_acceleration_type(vehicle), new_speed;    
    new_speed = acceleration * delta_time + vehicle->speed;
    vehicle->speed = new_speed >= 0 ? new_speed : 0.0;
    if (TRACE) printf("Moving at speed %lf\n", vehicle->speed);
}

/* Calculates the acceleration for a given vehicle at current point of time */
double calulate_acceleration_type(vehicle *vehicle){
    double acceleration = 0;
    switch (vehicle->cur_goal){
        case free_flow:
            acceleration = acceleration_free_flow(vehicle->max_acceleration, vehicle->speed);
            break;
        case follow_car:
            acceleration = acceleration_car_following(vehicle);
            break; 
        case red_light:
            acceleration = acceleration_car_following(vehicle);
            break; 
        case change_lane:
            /* Den skal være tom indtil vi får lavet alle andre "Must have" ting*/
            break; 
        case bus_stop:
            acceleration = acceleration_car_following(vehicle);
            break; 
        default:
            if (TRACE)printf("Goal not implemented\n");
            break;
    }
    return acceleration;
}

/* Returns acceleration using freeflow model */
double acceleration_free_flow(double max_acceleration, double current_speed){
    return max_acceleration * (1 - (current_speed/DESIRED_SPEED));
}

/* returns acceleration using car following model */
double acceleration_car_following(vehicle *vehicle){
    double current_speed = vehicle->speed, 
           front_speed = vehicle->speed_of_goal, 
           distance_diff = vehicle->distance_to_goal,
           acceleration,
           speed_diff = front_speed - current_speed,
           alpha = 1.1,
           beta =  (speed_diff >0 ? -0.2 : 0.9),
           gamma = (speed_diff >0 ? 0.2 : 1.0);
    if (distance_diff == 0. || (current_speed == 0.)){ /* Avoid zeroes to avoid infinities */
        return 0;
    }
    if (TRACE) printf("\nDOING: %lf * (%lf) / (%lf) * %lf = %lf", alpha, pow(current_speed, beta), pow(distance_diff,gamma), speed_diff, alpha * (pow(current_speed, beta) / (pow(distance_diff,gamma))) * speed_diff);
    acceleration = alpha * (pow(current_speed, beta) / (pow(distance_diff,gamma))) * speed_diff; 
    if (acceleration > vehicle->max_acceleration){
        acceleration = vehicle->max_acceleration;
    }
    return acceleration;
}

/* Find and set goal and distance for vehicle */
void find_goal_and_distance(vehicle *ptrvehicle){
    vertex *next_vertex;
    road *next_road;
    if (TRACE) printf("Finding goal and distance: ");
    /* Decision tree for what the current goal is */
    if (ptrvehicle->front_vehicle != NULL){
        /* If a car is in front of it, just follow it */
        set_goal_follow_next_car(ptrvehicle);
    }
    else{ /* If there is no car in front of it, figure out what the next vertex i */
        next_vertex = ptrvehicle->cur_road->destination;
        switch (next_vertex->vertex_type)
        {
            case v_crossing: /* if at crossing, stop if red or the out road is blocked,  */
                if (in_is_green(next_vertex, ptrvehicle->cur_road)){
                    next_road = get_next_road(ptrvehicle);
                    if (out_is_open(next_vertex, next_road)){
                        set_goal_to_next_road(ptrvehicle);
                    }else{
                        set_goal_stop_vertex(ptrvehicle);
                    }
                }else{
                    set_goal_stop_vertex(ptrvehicle);
                }
                break;
            case v_busstop: /* If at bus stop, avoid driving into exiting bus, and if bus, try to go to the busstop */
                if (!next_vertex->type.stop.bus_coming_out_bool){
                    if (ptrvehicle->type == vhcl_bus){
                        if (!next_vertex->type.stop.a_bus_at_the_stop){/* Bus stop not full? */
                            set_goal_bus_stop(ptrvehicle);
                        }else{
                            set_goal_to_next_road(ptrvehicle);
                        }
                    }else{
                        set_goal_to_next_road(ptrvehicle);
                    }
                }else{
                    set_goal_stop_vertex(ptrvehicle);
                }
                break;
            case v_endpoint: /* If endpoint, either check next road or just accelerate to the end */
                if(at_end_of_route(&ptrvehicle->cur_route)){
                    ptrvehicle->cur_goal = free_flow;
                    ptrvehicle->distance_to_goal = FREEFLOW_DEFAULT_DIST;
                }else{
                    set_goal_to_next_road(ptrvehicle);
                }
                break;
        }
    }
    /* If inside or in front of found goal, for example when a 2 cars enter a road close to eachother, wait until it is fixed */ 
    if (ptrvehicle->distance_to_goal < 0){
        set_goal_panic_wait(ptrvehicle);
    }
    if (TRACE) printf("Found goal %d with distance %lf, goalspeed: %lf\n", ptrvehicle->cur_goal, ptrvehicle->distance_to_goal, ptrvehicle->speed_of_goal);

}

/* Checks whether close enough, both in time and place, to use follow or freeflow */
goal freeflow_or_follow(vehicle *v, double distance){
    double time_headway = distance / v->speed;
    if (distance > MIN_FREEFLOW_DISTANCE && time_headway > v->desired_time_headway){
        return free_flow;
    }
    else{
        return follow_car;
    }
}

/* Gets distances to a car in front of it, assumes it exists */
double dist_to_front_car(vehicle *ptrvehicle){
    return ptrvehicle->front_vehicle->position - ptrvehicle->front_vehicle->length - ptrvehicle->position;
}
/* Gets distances to a vertex in front of it, assumes it and the current road exists */
double dist_to_front_vertex(vehicle *ptrvehicle){
    return ptrvehicle->cur_road->length - ptrvehicle->position;
}

/*---- These next functions simply set the goal to a specific obstacle */
void set_goal_stop_vertex(vehicle *ptrvehicle){
    ptrvehicle->distance_to_goal = dist_to_front_vertex(ptrvehicle);
    ptrvehicle->cur_goal = freeflow_or_follow(ptrvehicle, ptrvehicle->distance_to_goal);
    ptrvehicle->speed_of_goal = 0;
}
void set_goal_follow_next_car(vehicle *ptrvehicle){
    ptrvehicle->distance_to_goal = dist_to_front_car(ptrvehicle);
    ptrvehicle->cur_goal = freeflow_or_follow(ptrvehicle, ptrvehicle->distance_to_goal);
    ptrvehicle->speed_of_goal = ptrvehicle->front_vehicle->speed;

}
void set_goal_to_next_road(vehicle *ptrvehicle){
    road *next_road = get_next_road(ptrvehicle);
    double cur_road_dist = ptrvehicle->cur_road->length - ptrvehicle->position;
    if (next_road != NULL && next_road->vhcl_count > 0){ /* If there is a car on the next road, follow it */
        ptrvehicle->distance_to_goal = cur_road_dist + next_road->last_in_vhcl->position - next_road->last_in_vhcl->length;
        ptrvehicle->cur_goal = freeflow_or_follow(ptrvehicle, ptrvehicle->distance_to_goal);
        ptrvehicle->speed_of_goal = next_road->last_in_vhcl->speed;
    }
    else{ /* If the next road is empty, just accelerate freely */
        ptrvehicle->cur_goal = free_flow;
        ptrvehicle->distance_to_goal = FREEFLOW_DEFAULT_DIST;
        ptrvehicle->speed_of_goal = 0;
    }
}
void set_goal_bus_stop(vehicle *ptrvehicle){
    ptrvehicle->distance_to_goal = dist_to_front_vertex(ptrvehicle);
    ptrvehicle->cur_goal = freeflow_or_follow(ptrvehicle, ptrvehicle->distance_to_goal) == follow_car ? bus_stop : free_flow;
    if (ptrvehicle->cur_goal == bus_stop) ptrvehicle->distance_to_goal += 0.5;
    ptrvehicle->speed_of_goal = 0;
}
void set_goal_panic_wait(vehicle *ptrvehicle){
    ptrvehicle->distance_to_goal = 0;
    ptrvehicle->cur_goal = panic_wait;
    ptrvehicle->speed_of_goal = 0;
}
