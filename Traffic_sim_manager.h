#ifndef TRAFFIC_SIM_MANAGER
#define TRAFFIC_SIM_MANAGER

#define OwendhoIQ 0
#include "network_definitions.h"
typedef struct pathfinding_v pathfinding_v;
struct pathfinding_v{
    vertex *v;
    pathfinding_v *from;
    double value;
    int is_active;
};

int random_endpoint(int k);
void weight_in_endpoint(network *ptr_to_net, vertex **endpoints_in_array, int *possible_in_endpoints);
void weight_out_endpoint(network *ptr_to_net, vertex **endpoints_out_array, int *possible_out_endpoints);
void choose_in_out_vertex(vertex **endpoints_in_array, int possible_in_endpoints, vertex **endpoints_out_array, int possible_out_endpoints, vertex **vehicle_in_endpoint, vertex **vehicle_out_endpoint);
int random_endpoint(int possible_endpoints);

vehicle* dynamic_allocate_of_vehicle_array(int vehicle_count);
void make_all_car(network *net);
double random_max_acceleration_car();
double random_desired_time_headway();
double random_desired_time_lag_car();
double random_length_of_the_car();
double random_number_in_interval(double min_value, double max_value);
void random_route(network *net, route *route_out, vertex **endpoints_in, int endpoints_in_count, vertex **endpoints_out, int endpoints_out_count);

/* route *dijkstra(network *net, vertex *start_vertex, vertex *end_vertex);
int get_index_of_element_in_array(void *array_start, void* array_cur, int element_size);
 */
void create_route_dijkstra(network *net, vertex *start_vertex, vertex *end_vertex, route *route_out, road_type type);
int index_of_vert_from_pointer(network *net, vertex *vp);
pathfinding_v *min_active_vertex(pathfinding_v *pf_vertices, int count);
#endif
