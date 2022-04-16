
#ifndef VEHICLE_BEHAVIOR
#define VEHICLE_BEHAVIOR
#define DESIRED_SPEED 13.9
#define MIN_FREEFLOW_DISTANCE 2
#define FREEFLOW_DEFAULT_DIST 9999

#include "network_definitions.h"

goal freeflow_or_follow(vehicle *v, double distance);
double dist_to_front_car(vehicle *ptrvehicle);
double dist_to_front_vertex(vehicle *ptrvehicle);
void set_goal_stop_vertex(vehicle *ptrvehicle);
void set_goal_follow_next_car(vehicle *ptrvehicle);
void set_goal_bus_stop(vehicle *ptrvehicle);
void set_goal_panic_wait(vehicle *ptrvehicle);
int check_for_redlight(crossing *ptr_to_crossing);

int at_end_of_route(route *rte);
road *road_from_to(vertex *v1, vertex *v2, road_type type);
void remove_vehicle(vehicle *vhcl, double cur_time);
int try_use_out_road(vertex *vert, road *rd);
void reset_out_road(vertex *vert);
void vehicle_move_road(vehicle *vhcl, road *next_road);
void move_vehicle(vehicle *vhcl, double delta_time, double cur_time);
void new_speed(vehicle* vehicle, double delta_time);
double calulate_acceleration_type(vehicle* vehicle);
double acceleration_free_flow(double max_acceleration, double current_speed);
double acceleration_car_following(vehicle *vehicle);
void headway_distance(vehicle *ptrvehicle);
int check_for_redlight(crossing *ptrcrossing);
void find_goal_and_distance(vehicle *ptrvehicle);
void set_goal_to_next_road(vehicle *ptrvehicle);
#endif
