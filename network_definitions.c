/*
 * Make functions for all structs in the header file network_structs.h 
 * Made by SW1A402 in P1 year 2020
 * 
 * Purpose: Holds functions that create a struct of a given type, with input as values,
 *              and sets other variables to their default values.
 */

#include <stdio.h>
#include "stdlib.h"
#include "network_definitions.h"

vehicle make_vehicle(vehicle_type type, double max_acceleration, double desired_time_headway, double desired_time_lag, double length, route *cur_route)
{
    vehicle vhcl;
    vhcl.type = type;
    vhcl.max_acceleration = max_acceleration;
    vhcl.desired_time_headway = desired_time_headway;
    vhcl.desired_time_lag = desired_time_lag;
    vhcl.length = length;
    vhcl.cur_route = *cur_route;

    vhcl.position = 0;
    vhcl.cur_road = NULL;
    vhcl.front_vehicle = NULL;
    vhcl.cur_goal = free_flow;
    vhcl.speed = 0;
    vhcl.distance_to_goal = 0;
    vhcl.speed_of_goal = 0;
    vhcl.cur_activity = active_at_start;
    vhcl.spawn_time = 0;
    return vhcl;
}

vehicle make_bus(vehicle_type type, double max_acceleration, double desired_time_headway, double desired_time_lag, double length)
{
    vehicle vhcl;
    vhcl.type = type;
    vhcl.max_acceleration = max_acceleration;
    vhcl.desired_time_headway = desired_time_headway;
    vhcl.desired_time_lag = desired_time_lag;
    vhcl.length = length;

    vhcl.position = 0;
    vhcl.cur_road = NULL;
    vhcl.front_vehicle = NULL;
    vhcl.cur_goal = free_flow;
    vhcl.speed = 0;
    vhcl.distance_to_goal = 0;
    vhcl.speed_of_goal = 0;
    vhcl.cur_activity = active_at_start;
    vhcl.spawn_time = 0;
    return vhcl;
}

vertex make_base_vertex(road *roads_in[MAX_IN_OUT_ROADS], int roads_in_count, road *roads_out[MAX_IN_OUT_ROADS], int roads_out_count){
    vertex v;
    int i;
    v.roads_in_count = roads_in_count;
    v.roads_out_count = roads_out_count;
    for (i = 0; i < roads_in_count; i++)
    {
        v.roads_in[i] = roads_in[i];
    }
        for (i = 0; i < roads_out_count; i++)
    {
        v.roads_out[i] = roads_out[i]; 
    }

    return v;
}

vertex make_crossing(road *roads_in[MAX_IN_OUT_ROADS], int roads_in_count, road *roads_out[MAX_IN_OUT_ROADS], int roads_out_count, int phasecount, int in_roads_phases[MAX_IN_OUT_ROADS], int phase_times[MAX_PHASES]){
    vertex v = make_base_vertex(roads_in, roads_in_count, roads_out, roads_out_count);
    int i;
    v.vertex_type = v_crossing;
    v.type.cross.phasecount = phasecount;
    for (i = 0; i < phasecount; i++)
    {
        v.type.cross.phase_times[i] = phase_times[i];
    }
    
    for (i = 0; i < MAX_IN_OUT_ROADS; i++)
    {
        v.type.cross.in_roads_phases[i] = in_roads_phases[i];
        v.type.cross.out_road_open[i] = 1;
    }

    v.type.cross.current_phase = 0;
    v.type.cross.cur_timer = phase_times[0];
    return v;
}

vertex make_endpoint(road *roads_in[MAX_IN_OUT_ROADS], int roads_in_count, road *roads_out[MAX_IN_OUT_ROADS], int roads_out_count, int weight_in, int weight_out){
    vertex v = make_base_vertex(roads_in, roads_in_count, roads_out, roads_out_count);
    v.vertex_type = v_endpoint;
    v.type.end.weight_in = weight_in;
    v.type.end.weight_out = weight_out;
    return v;
}

vertex make_busstop(road *roads_in[MAX_IN_OUT_ROADS], int roads_in_count, road *roads_out[MAX_IN_OUT_ROADS], int roads_out_count, double people_per_second){
    vertex v = make_base_vertex(roads_in, roads_in_count, roads_out, roads_out_count);
    v.vertex_type = v_busstop;
    v.type.stop.people_per_sec = people_per_second;

    v.type.stop.cur_waiting = 0;
    v.type.stop.cur_waiting_time = 0;
    v.type.stop.a_bus_at_the_stop = 0;
    v.type.stop.bus_coming_out_bool = 0;
    v.type.stop.bus = NULL;
    return v;
}

busroute make_busroute(route the_route, double interval){
    busroute b_busroute;
    b_busroute.b_route = the_route;
    b_busroute.bus_interval = interval;
    b_busroute.timer = 0;
    return b_busroute;
}


road make_road(road_type type, vertex *destination, double length, int lanes)
{
    road r;
    r.type = type;
    r.length = length;
    r.lanes = lanes;
    r.destination = destination;
    r.vhcl_count = 0;
    r.last_in_vhcl = NULL;
    return r;
}

route make_route(vertex **points, int route_length)
{
    route r;
    r.points = points;
    r.route_length = route_length;

    r.cur_point = 0;
    return r;
}

network make_network(road *roads, int road_count, vehicle *vehicles, int vehicle_count, vertex *vertices, int vertex_count, int car_spawn_period){
    network n;
    n.roads = roads;
    n.vehicles = vehicles;
    n.vertices = vertices;
    n.road_count = road_count;
    n.vertex_count = vertex_count;
    n.vehicle_count = vehicle_count;
    n.car_spawn_period = car_spawn_period;
    return n;
}
