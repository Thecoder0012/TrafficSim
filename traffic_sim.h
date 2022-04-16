#ifndef TRAFFIC_SIM
#define TRAFFIC SIM

#define TRACE 0
#define SEEDED 1
#define BRT 0

#include "framework.h"

enum data_columns
{
    data_bus_north_travel_time,
    data_bus_north_headway,
    data_bus_south_travel_time,
    data_bus_south_headway,
    data_car_travel_time,
    data_sim_time,
    data_car_count,
    data_stopped_cars,
    data_average_speed
};

void find_goal_and_distance_all_vehicles(network *net);
void update_speed_all_vehicles(network* net, double delta);
void move_all_vehicles(network* net, double delta, double sim_time);
int simulation_step(int iteration, double delta, double time, void *sim_data);
void prompt_car_count_and_time(int *car_count, int *spawn_time);
void clear_line(void);
void debug_print_road(road *rd);
void simulate_all_vehicles(network *net, double delta, double cur_time);
void simulate_all_vertices(network *net, double delta);
int should_continue(network *net);
int prompt_seed(void);
void log_car_speed_and_count(network *net);

#endif
