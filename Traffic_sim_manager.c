/*
 * Managing the creation of cars, and creating their random routes, for the traffic sim
 * Made by SW1A402 in P1 year 2020
 * 
 * Purpose: Has all functions related to creating a random car
 *              Can create a car with random values assigned
 *              Can pick two random, weighted, endpoints
 *              Can generate a route between two random points
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <assert.h>

#include "Traffic_sim_manager.h"
#include "network_definitions.h"
#include "traffic_sim.h"

#define MIN_MAX_ACCELERATION_CAR 1.3 /* m/s^2 */
#define MAX_MAX_ACCELERATION_CAR 2.0 /* m/s^2 */

#define MIN_DESIRED_TIME_HEADWAY 2.6  /* s */
#define MAX_DESIRED_TIME_HEADWAY 3.17 /* s */

#define MIN_DESIRED_TIME_LAG 0 /* s */
#define MAX_DESIRED_TIME_LAG 0 /* s */

#define MIN_LENGHT_OF_CAR 4 /* m Meter */
#define MAX_LENGHT_OF_CAR 4.7 /* m Meter */

/* ----------------------------------Choose routes section start-------------------------------------- */
/* Creates a pointer array where each endpoint is duplicated by its in weight, to allow weighted randomizing */
void weight_in_endpoint(network *ptr_to_net, vertex **endpoints_in_array, int *possible_in_endpoints)
{
    int vertex_count = ptr_to_net->vertex_count;
    int i, j, k = 0;
    int weight;
    vertex *ptr_to_vertex = ptr_to_net->vertices;

    for (i = 0; i < vertex_count; i++)
    {
        if (ptr_to_vertex[i].vertex_type == v_endpoint)
        {
            weight = ptr_to_vertex[i].type.end.weight_in;
            for (j = 1; j <= weight; j++)
            {
                endpoints_in_array[k] = &ptr_to_vertex[i];
                k++;
            }
        }
    }
    *possible_in_endpoints = k;
}

/* Creates a pointer array where each endpoint is duplicated by its out weight, to allow weighted randomizing */
void weight_out_endpoint(network *ptr_to_net, vertex **endpoints_out_array, int *possible_out_endpoints)
{
    int vertex_count = ptr_to_net->vertex_count;
    int i, j, k = 0;
    int weight;
    vertex *ptr_to_vertex = ptr_to_net->vertices;

    for (i = 0; i < vertex_count; i++)
    {
        if (ptr_to_vertex[i].vertex_type == v_endpoint)
        {
            weight = ptr_to_vertex[i].type.end.weight_out;
            for (j = 1; j <= weight; j++)
            {
                endpoints_out_array[k] = &ptr_to_vertex[i];
                k++;
            }
        }
    }
    *possible_out_endpoints = k;
}

/* Choose start and exit point for a random route */
void choose_in_out_vertex(vertex **endpoints_in_array, int possible_in_endpoints, vertex **endpoints_out_array, int possible_out_endpoints, vertex **vehicle_in_endpoint, vertex **vehicle_out_endpoint)
{
    int index;
    /* In endpoint */
    index = random_endpoint(possible_in_endpoints);
    *vehicle_in_endpoint = endpoints_in_array[index];
    /* out endpoint */
    do{
        index = random_endpoint(possible_out_endpoints);
    } while (*vehicle_in_endpoint == endpoints_out_array[index]);
    *vehicle_out_endpoint = endpoints_out_array[index];
}

/* Returns index for an endpoint  */
int random_endpoint(int possible_endpoints)
{
    return rand() % possible_endpoints;
}

/* ----------------------------------Choose routes section end-------------------------------------- */

/* ----------------------------------Randomize car section start-------------------------------------- */
/* Allocates memory for cars to be spawned, and return pointer to array */
vehicle *dynamic_allocate_of_vehicle_array(int vehicle_count)
{
    vehicle *vehicle_array = (vehicle *)malloc(vehicle_count * sizeof(vehicle));
    if (vehicle_array == NULL){
        printf("Cannot allocate enough memory. Bye");
        exit(EXIT_FAILURE);
    }
    return vehicle_array;
}

/* Creates all cars that will be driving through the network, and assigns random routes and variables */
void make_all_car(network *net)
{
    int i, vehicle_count = net->vehicle_count;
    /* DECLARATION OF PARAMETERS TO IN AND OUT ENDPOINTS */
    vertex *endpoints_in_array[1000];
    vertex *endpoints_out_array[1000];
    int possible_in_endpoints, possible_out_endpoints;
    route new_route;
    /* FIND WEIGHT IN AND OUT ENDPOINTS */
    weight_in_endpoint(net, endpoints_in_array, &possible_in_endpoints);
    weight_out_endpoint(net, endpoints_out_array, &possible_out_endpoints);
    for (i = 0; i < vehicle_count; i++)
    {
        random_route(net, &new_route, endpoints_in_array, possible_in_endpoints, endpoints_out_array, possible_out_endpoints);
        net->vehicles[i] = make_vehicle(vhcl_car, random_max_acceleration_car(), random_desired_time_headway(), random_desired_time_lag_car(), random_length_of_the_car(), &new_route);
    }
}

/* These functions return a random value between defined intervals */
double random_max_acceleration_car(){
    return random_number_in_interval(MIN_MAX_ACCELERATION_CAR, MAX_MAX_ACCELERATION_CAR);
}

double random_desired_time_headway(){
    return random_number_in_interval(MIN_DESIRED_TIME_HEADWAY, MAX_DESIRED_TIME_HEADWAY);
}

double random_desired_time_lag_car(){
    /* Hvis vi får lavet Lane change, skal vi bruge dette her */
    /* return random_number_in_interval(MIN_DESIRED_TIME_LAG, MAX_DESIRED_TIME_LAG); */
    return 0.0;
}

double random_length_of_the_car(){
    return random_number_in_interval(MIN_LENGHT_OF_CAR, MAX_LENGHT_OF_CAR);
}

/* Returns random double in interval with 3 decimals */
double random_number_in_interval(double min_value, double max_value)
{
    int min_value_1000 = (int)(min_value * 1000),
        max_value_1000 = (int)(max_value * 1000),
        random_number;

    random_number = rand() % (max_value_1000 - min_value_1000) + min_value_1000; 

    return (double)random_number / 1000;
}

/* ----------------------------------Randomize car section end-------------------------------------- */


/* ----------------------------------Pathfinding section start-------------------------------------- */
/* Returns a random route for the network */
void random_route(network *net, route *route_out, vertex **endpoints_in, int endpoints_in_count, vertex **endpoints_out, int endpoints_out_count){
    vertex *startp = NULL;
    vertex *finishp = NULL;
    if (TRACE) printf("making random route\n");
    choose_in_out_vertex(endpoints_in, endpoints_in_count, endpoints_out, endpoints_out_count, &startp, &finishp);
    if (TRACE) printf("in/out chosen\n");

    create_route_dijkstra(net, startp, finishp, route_out, road_any);
    if (TRACE) printf("route created\n");

}

/* Uses dijkstras pathfinding algorithm to create the shortest route from 2 points */
void create_route_dijkstra(network *net, vertex *start_vertex, vertex *end_vertex, route *route_out, road_type type){
    pathfinding_v *pathfinding_vertices = (pathfinding_v*)malloc(net->vertex_count * sizeof(pathfinding_v));
    int i, route_count;
    pathfinding_v *cur_vert, *end_pf_vertex;
    double new_value;
    if (TRACE) printf("Dijkstra start\n");
    /* Setting up all vertices */
    for (i = 0; i < net->vertex_count; i++)
    {
        pathfinding_vertices[i].from = NULL;
        pathfinding_vertices[i].v = net->vertices + i;
        if (net->vertices + i == start_vertex){
            pathfinding_vertices[i].is_active = 1;
            pathfinding_vertices[i].value = 0.;
        }
        else{
            pathfinding_vertices[i].is_active = 0;
            pathfinding_vertices[i].value = __DBL_MAX__;
        }
    }
    if (TRACE) printf("Setup complete\n");

    /* Run dijkstras algorithm to generate fastest paths to destination for many ppoints */
    do{
        cur_vert = min_active_vertex(pathfinding_vertices, net->vertex_count); /* Get the lowest valued vertex */
        if (cur_vert->v == end_vertex) break; /* If destination is current lowest values vertex, stop finding paths */
        for (i = 0; i < cur_vert->v->roads_out_count; i++)
        {
            if (cur_vert->v->roads_out[i]->type == road_any || cur_vert->v->roads_out[i]->type == type){/* Check if road can be used by this vehicle */
                pathfinding_v *neighbor = pathfinding_vertices + index_of_vert_from_pointer(net, cur_vert->v->roads_out[i]->destination);
                new_value = cur_vert->v->roads_out[i]->length + cur_vert->value;
                if (neighbor->value > new_value){
                    neighbor->value = new_value;
                    neighbor->is_active = 1;
                    neighbor->from = cur_vert;
                }
            }
        }
        cur_vert->is_active = 0;
    } while (cur_vert->v != end_vertex);
    if (TRACE) printf("Paths generated\n");
    /* Create route by following backwards */
    /* First count the length of route */
    end_pf_vertex = pathfinding_vertices + index_of_vert_from_pointer(net, end_vertex);
    if (TRACE) printf("Distance: %lf\n", end_pf_vertex->value);
    cur_vert = end_pf_vertex;
    route_count = 1;
    while (cur_vert->from != NULL)
    {
        cur_vert = cur_vert->from;
        route_count++;
    }

    /* Allocate space for the route array */
    route_out->route_length = route_count;
    route_out->cur_point = 0;
    route_out->points = (vertex **)malloc(route_count * sizeof(vertex*));
    cur_vert = end_pf_vertex;
    if (TRACE) printf("Allocated\n");
    /* Follow the path backwards and assign vertices to route array */
    for (i = route_count - 1; i >= 0; i--)
    {
        route_out->points[i] = cur_vert->v;
        cur_vert = cur_vert->from;
    }
    if (TRACE) printf("Route assigned and complete\n");
    free(pathfinding_vertices); /* Free temporary pathfinding vertex array */
}

/* Returns the current pathfinding vertex with lowest value (currently shortest path) that is active */
pathfinding_v *min_active_vertex(pathfinding_v *pf_vertices, int count)
{
    pathfinding_v *cur_min = NULL;
    int i;
    for (i = 0; i < count; i++)
    {
        if ((cur_min == NULL || pf_vertices[i].value < cur_min->value) && pf_vertices[i].is_active){
            cur_min = pf_vertices + i;
        }
    }
    return cur_min;
}

/* Gets the index of a vertex in the network vertices array from its current pointer
    Assumes pointer is in vertex pointer array. Slow way of getting index. O(n). */
int index_of_vert_from_pointer(network *net, vertex *vp){
    int i;
    for (i = 0; i < net->vertex_count; i++)
    {
        if (net->vertices + i == vp)
        {
            return i;
        }
    }
    /* If it doesnt exist, something went wrong at the caller */
    printf("\nCouldnt find index of vertex from pointer\n");
    exit(EXIT_FAILURE);
    return 0;
}
