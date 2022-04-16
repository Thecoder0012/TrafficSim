/*
 * Contains the very general simulation logic, contains main
 * Made by SW1A402 in P1 year 2020
 * 
 * Purpose: Defines what is done when in the simulation, and prompts user.
 *              Has main
 *              Communicates with user
 *              Starts and stops the simulation
 *              Defines the order of what gets simulated when
 */


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include "framework.h"
#include "network_definitions.h"
#include "vehicle_lists.h"
#include "vehicle_behavior.h"
#include "Traffic_sim_manager.h"
#include "network_reader.h"
#include "Bus_behaviour.h"
#include "spawn_cars.h"

#include "traffic_sim.h"
#include "add_remove_vehicle.h"
#include "crossing.h"

#define SPD_LIMIT 13.9           /* Speed limit in m/s */
#define BOARD_TIME_PASSENGER 2.3 /* Time for a single passenger to board a bus */
#define DOOR_TIME 6              /* Combined bus door opening and closing time */
#define DELTA_TIME 0.2
#define ARRAY_SIZE 1000
#define DATA_LOG_INTERVAL 10
#define MAX_BUS_COUNT 300

const char* column_names[DATA_COLUMNS] = {"BusNorth travel times", "BusNorth headway","BusSouth travel times","BusNorth headway", "Car travel times","sim time", "car count", "slow cars", "average speed", "", "", "","","","",""};

int main(void)
{
    int car_count;
    int spawn_time;
    int seed;
    network rd_network = read_file_and_allocate_to_network();

    if (SEEDED){
        seed = prompt_seed();
    }
    else{
        seed = time(NULL);
    }
    srand(seed);
    printf("Currently on seed: \"%d\"\n", seed);
    
    sim_init();
    set_data_columns(column_names);

    printf("Welcome to the traffic simulation!\n");
    prompt_car_count_and_time(&car_count, &spawn_time);
    rd_network.car_spawn_period = spawn_time;

    rd_network.vehicles = dynamic_allocate_of_vehicle_array(car_count);
    rd_network.vehicle_count = car_count;
    make_all_car(&rd_network);

    rd_network.buses = dynamic_allocate_of_vehicle_array(MAX_BUS_COUNT);
    rd_network.bus_count = MAX_BUS_COUNT;
    make_all_buses(&rd_network);

    sim_loop(simulation_step, DELTA_TIME, (void *)(&rd_network));
    
    printf("\nEnding and exporting data...\n");

    sim_end();
    
    return EXIT_SUCCESS;
}

/* This function is called every simulation step */
int simulation_step(int iteration, double delta, double sim_time, void *sim_data){
    network *net = (network *)sim_data;
    timed_vehicle_spawner(net,sim_time,net->car_spawn_period);
    timed_bus_spawner(net,sim_time, delta);
    if (TRACE) printf("    simulating vehicles");
    simulate_all_vehicles(net, delta, sim_time);
    if (TRACE) printf("    simulating vertices\n");
    simulate_all_vertices(net, delta);    
    if (TRACE) printf("------------step complete--------------\n");

    if (iteration % DATA_LOG_INTERVAL == 0){
        sim_log_data(data_sim_time, sim_time);
        log_car_speed_and_count(net);
    }
    return should_continue(net);
    /* This outcommented code is used to show a specific road for debugging purposes */    
    /* if (net->roads[13].vhcl_count > 0)
    {
        debug_print_road(net->roads + 13);
    } */
    /* This outcommented code follows a car in the simulation for debugging purposes */
    /* if ( net->vehicles[0].cur_activity == active && TRACE)
    {
        printf("car, Move. Cur pos %lf in part %d of route\n", net->vehicles[0].position, net->vehicles[0].cur_route.cur_point);
        debug_print_road(net->vehicles[0].cur_road);
    }*/
}



/* Simulates all vehicles for one step with length delta */
void simulate_all_vehicles(network *net, double delta, double sim_time){
    if (TRACE) printf(", Distance\n");
    find_goal_and_distance_all_vehicles(net);
    if (TRACE) printf(", Accel\n");
    update_speed_all_vehicles(net, delta);
    if (TRACE) printf(", Move\n");
    move_all_vehicles(net, delta, sim_time);
}

/* Iterate over all vehicles and find distances */
void find_goal_and_distance_all_vehicles(network *net){
    int i;
    /* Buses */
    for (i = 0; i < net->bus_count; i++)
    {
        if (net->buses[i].cur_activity == active)
            find_goal_and_distance(&net->buses[i]);
    } 
    /* Cars */
    for (i = 0; i < net->vehicle_count; i++)
    {
        if (net->vehicles[i].cur_activity == active)
            find_goal_and_distance(&net->vehicles[i]);
    }    
}

/* Iterate over all vehicles and accelerate them */
void update_speed_all_vehicles(network* net, double delta){
    int i;
    /* Buses */
    for (i = 0; i < net->bus_count; i++)
    {
        if (net->buses[i].cur_activity == active)
            new_speed(&net->buses[i], delta);
    }
    /* Cars */
    for (i = 0; i < net->vehicle_count; i++)
    {
        if (net->vehicles[i].cur_activity == active)
            new_speed(&net->vehicles[i], delta);
    }
}

/* Iterate over all vehicles and move them */
void move_all_vehicles(network* net, double delta, double sim_time){
    int i;
    /* Buses */
    for (i = 0; i < net->bus_count; i++)
    {
        if (net->buses[i].cur_activity == active){
            move_vehicle(&net->buses[i], delta, sim_time);
        }
    }
    /* Cars */
    for (i = 0; i < net->vehicle_count; i++)
    {
        if (net->vehicles[i].cur_activity == active)
            move_vehicle(&net->vehicles[i], delta, sim_time);
    }
}

/* Simulates all vertices for one step of length delta */
void simulate_all_vertices(network *net, double delta)
{
    int i;
    vertex *v;
    vertex_type v_type;
    for (i = 0; i < net->vertex_count; i++)
    {
        v = net->vertices + i;
        v_type = v->vertex_type;
        if (v_type == v_busstop){
            bus_stop_function(&v->type.stop, delta);
        }
        else if (v_type == v_crossing){
            update_crossing(v, delta);
        }
    }
}

/* Returns SIM_STOP if the stop condition of having no active cars is met, otherwise returns SIM_CONTINUE */
int should_continue(network *net){
    int i;
    for (i = 0; i < net->vehicle_count; i++)
    {
        if(net->vehicles[i].cur_activity != inactive){
            return SIM_CONTINUE;
        }
    }
    return SIM_STOP;
}

/* Prompts for amount of cars */
void prompt_car_count_and_time(int *car_count, int *spawn_time)
{
    int success;
    do{
        printf("Total amount of cars (x) to be simulated over y seconds time (x y)> ");
        success = scanf(" %d %d", car_count, spawn_time);
        if (!success){
            clear_line();
        }
    } while (!success);
}

/* Prompts for seed */
int prompt_seed(void)
{
    int success, seed;
    do{
        printf("Integer seed> ");
        success = scanf(" %d", &seed);
        if (!success){
            clear_line();
        }
    } while (!success);
    return seed;
}

/* Clear current line of the input0 */
void clear_line(void)
{
    char read_char;
    do{
        scanf("%c", &read_char);
    } while (read_char != '\n');
}

void log_car_speed_and_count(network *net){
    int i, active_cars = 0, slow_cars = 0;
    double speed_sum = 0;
    for (i = 0; i < net->vehicle_count; i++)
    {
        if (net->vehicles[i].cur_activity == active){
            speed_sum += net->vehicles[i].speed;
            active_cars++;
            if (net->vehicles[i].speed < 0.5){
                slow_cars++;
            }
        }
    }
    speed_sum /= active_cars;
    sim_log_data(data_average_speed, speed_sum);
    sim_log_data(data_car_count, active_cars);
    sim_log_data(data_stopped_cars, (double)slow_cars / (double)active_cars);
}

/* Prints out a road and all its cars to stdout for debugging purposes */
void debug_print_road(road *rd)
{
    int char_length = 50, i, found_vehicles = 0;
    double cur_pos = 0, last_pos = 0;
    vehicle *cur_vehicle;
    for (i = 0; i < char_length + 2; i++)
    {
        printf("_");
    }

    printf("\nRoad visualiser. Should be %d cars. Road is %.0lf meters.", rd->vhcl_count, rd->length);
    printf("\n|");
    for (i = 0; i < char_length; i++)
    {
        printf("-");
    }
    printf("|\n>");
    cur_vehicle = rd->last_in_vhcl;
    for (i = 0; i < char_length; i++)
    {
        found_vehicles = 0;
        while (cur_vehicle != NULL && cur_vehicle->position >= last_pos && cur_vehicle->position <= cur_pos)
        {
            found_vehicles++;
            cur_vehicle = cur_vehicle->front_vehicle;
        }
        if (found_vehicles > 0){
            printf("%d", found_vehicles);
        }
        else{
            printf(" ");
        }
        last_pos = cur_pos;
        cur_pos += rd->length / ((double)char_length - 1);
    }
    printf("\n|");
    for (i = 0; i < char_length; i++)
    {
        printf("-");
    }
    printf("|\n\n");
}
