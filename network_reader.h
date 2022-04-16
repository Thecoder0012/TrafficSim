
#ifndef NETWORK_READER
#define NETWORK_READER
#include "network_definitions.h"
#include <stdio.h>

#define MAX_LINE_LENGTH 100
#define MAX_LABEL_LENGTH 100

#define MAX_FILENAME_LENGTH 100

network read_file_and_allocate_to_network(void);
char *get_valid_line(char* out, FILE *fp);
road read_road_line(const char *line, const network *net, road *this_ptr, char (*vertex_labels)[MAX_LABEL_LENGTH], char (*road_labels)[MAX_LABEL_LENGTH]);
vertex read_crossing_line(const char *line, const network *net, int index, char (*vertex_labels)[MAX_LABEL_LENGTH], char (*road_labels)[MAX_LABEL_LENGTH]);

void find_in_roads_phases(int *in_roads_phases, const char *string_in_roads_phases, char* v_label, char (*vertex_labels)[MAX_LABEL_LENGTH], char (*road_labels)[MAX_LABEL_LENGTH], const network *net);
int find_number_of_in_roads(const char *string_in_roads_phases);
void find_road_label(char *road_label, int *phase, int *element, const char *string_in_roads_phases);

vertex read_endpoint_line(const char *line, const network *net, int index, char (*vertex_labels)[MAX_LABEL_LENGTH], char (*road_labels)[MAX_LABEL_LENGTH]);
vertex read_busstop_line(const char *line, const network *net, int index, char (*vertex_labels)[MAX_LABEL_LENGTH], char (*road_labels)[MAX_LABEL_LENGTH]);
void get_structure_counts(FILE *fp, int *vertex_count, int *road_count, int *busroute_count);
void get_structure_labels(FILE *fp, char (*v_labels)[MAX_LABEL_LENGTH], char (*r_labels)[MAX_LABEL_LENGTH]);
void read_label(const char *line, char *label_out);
int index_of_string(char (*label_array)[MAX_LABEL_LENGTH], int label_array_count, char *label);

void populate_network(FILE *fp, network *net, char (*vertex_labels)[MAX_LABEL_LENGTH], char (*road_labels)[MAX_LABEL_LENGTH]);

busroute read_busroute_label(const char *line, const network *net, char (*vertex_labels)[MAX_LABEL_LENGTH]);
route make_the_bus_route(const char *line, const network *net, char (*vertex_labels)[MAX_LABEL_LENGTH]);
int count_number_of_vertices(const char *line);
void find_vertices_on_busroute(int number_of_vertices,char* vertices_line,const network* net,char(*vertex_labels)[MAX_LABEL_LENGTH], route* busroute);
void read_vertex(const char *vertices_line, char *vertex, int *element);
double initialize_bus_interval(const char *line);

#endif
