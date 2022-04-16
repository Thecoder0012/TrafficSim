/*
 * Reads a network file chosen by user to a network struct.
 * Made by SW1A402 in P1 year 2020
 * 
 * Purpose: Creating the network struct (containing vertices, roads, etc), from a txt file
 */

#include <stdio.h>
#include <stdlib.h>
#include "network_definitions.h"
#include "network_reader.h"
#include "traffic_sim.h"
#include "string.h"
#include "assert.h"
#include "crossing.h"

/* Read entire network file and return it as a network struct
    Does not handle vehicles */
network read_file_and_allocate_to_network(void)
{
    /* Initialize all variables */
    FILE *fp;

    vertex *vertices;
    road *roads;
    busroute *busroutes;
    network net;
    char(*vertex_labels)[MAX_LABEL_LENGTH];
    char(*road_labels)[MAX_LABEL_LENGTH];
    char filename[MAX_FILENAME_LENGTH];
    int road_count = 0, vertex_count = 0, busroute_count = 0;
    printf("Enter network file filename> ");
    scanf(" %s", filename);
    fp = fopen(filename, "r");
    if (fp == NULL){
        printf("Couldn't open network file %s. Did you remember the file ending? Exiting.\n", filename);
        exit(EXIT_SUCCESS);
    }
    
    /* Get amount of structures */
    get_structure_counts(fp, &vertex_count, &road_count, &busroute_count);
 
    /* Get labels */
    vertex_labels = malloc(vertex_count * MAX_LABEL_LENGTH);
    road_labels = malloc(road_count * MAX_LABEL_LENGTH);
    get_structure_labels(fp, vertex_labels, road_labels);

    /* Allocate vertex and road struct arrays */
    vertices = (vertex *)calloc(vertex_count, sizeof(vertex));
    roads = (road *)calloc(road_count, sizeof(road));
    busroutes = (busroute *)calloc(busroute_count, sizeof(busroute));

    net.roads = roads;
    net.vertices = vertices;
    net.busroutes = busroutes;

    net.road_count = road_count;
    net.vertex_count = vertex_count;
    net.busroute_count = busroute_count;
    net.vehicle_count = 0;

    populate_network(fp, &net, vertex_labels, road_labels);
    return net;
}

/* Reads network file and gets the amount of vertices, roads and busroutes */
void get_structure_counts(FILE *fp, int *vertex_count, int *road_count, int *busroute_count)
{
    char line[MAX_LINE_LENGTH];
    *vertex_count = 0;
    *road_count = 0;
    *busroute_count = 0;
    while (get_valid_line(line, fp) != NULL)
    {
        switch (line[0])
        {
            case 'r':
                (*road_count)++;
                break;
            case 'c': case 'e': case 'b':
                (*vertex_count)++;
                break;
            case 'x':
                (*busroute_count)++;
                break;
            default:
                printf("Undefined structure?\n");
                break;
        }
    }
    rewind(fp); /* Go back to the start of file again */
}

/* Reads all labels (the unique string identifier) for all lines */
void get_structure_labels(FILE *fp, char (*v_labels)[MAX_LABEL_LENGTH], char (*r_labels)[MAX_LABEL_LENGTH])
{
    char line[MAX_LINE_LENGTH];
    int cur_v_label = 0;
    int cur_r_label = 0;
    while (get_valid_line(line, fp) != NULL)
    {
        switch (line[0])
        {
        case 'r':
            read_label(line, r_labels[cur_r_label]);
            cur_r_label++;
            break;
        case 'b': case 'e': case 'c':
            read_label(line, v_labels[cur_v_label]);
            cur_v_label++;
            break;
        case 'x':
            /* Busroutes don´t need labels */
            break;
        default:
            printf("Undefined structure?\n");
            break;
        }
    }
    rewind(fp); /* Go back to the start of file again */
}

/* Insert all roads and vertices in the network, assumes that arrays are properly allocated beforehand */
void populate_network(FILE *fp, network *net, char (*vertex_labels)[MAX_LABEL_LENGTH], char (*road_labels)[MAX_LABEL_LENGTH])
{
    int cur_road_i = 0, cur_vertex_i = 0, actual_v_index = 0, cur_busroute_i = 0;
    char line[MAX_LINE_LENGTH];
    while (get_valid_line(line, fp) != NULL)
    {
        switch (line[0])
        {
            case 'r':
                if(TRACE)printf("cur_road_i: %d\n", cur_road_i);
                net->roads[cur_road_i] = read_road_line(line, net, net->roads+cur_road_i, vertex_labels, road_labels);
                cur_road_i++;
                break;
            case 'c':
                if(TRACE)printf("cur_vertex_i: %d\n", cur_vertex_i);
                actual_v_index = index_of_string(vertex_labels, net->vertex_count, vertex_labels[cur_vertex_i]);
                net->vertices[actual_v_index] = read_crossing_line(line, net, actual_v_index, vertex_labels, road_labels);
                cur_vertex_i++;
                break;
            case 'e':
                if(TRACE)printf("cur_vertex_i: %d\n", cur_vertex_i);
                actual_v_index = index_of_string(vertex_labels, net->vertex_count, vertex_labels[cur_vertex_i]);
                net->vertices[actual_v_index] = read_endpoint_line(line, net, actual_v_index, vertex_labels, road_labels);
                cur_vertex_i++;
                break;
            case 'b':
                if(TRACE)printf("cur_vertex_i: %d\n", cur_vertex_i);
                actual_v_index = index_of_string(vertex_labels, net->vertex_count, vertex_labels[cur_vertex_i]);
                net->vertices[actual_v_index] = read_busstop_line(line, net, actual_v_index, vertex_labels, road_labels);
                cur_vertex_i++;
                break;
            case 'x':
                if(TRACE)printf("cur_busroute_i: %d\n", cur_busroute_i);
                net->busroutes[cur_busroute_i] = read_busroute_label(line, net, vertex_labels);
                cur_busroute_i++;
                break;

            default:
                printf("Struct of type '%c' not implemented in network reader!\n", line[0]);
                exit(EXIT_FAILURE);
                break;
        }
    }
}

/* Read the label from a line */
void read_label(const char *line, char *label_out)
{
    sscanf(line, "%*c:%[^{]", label_out);
}

/* Read lines until a valid line is found, and return it */
char *get_valid_line(char *out, FILE *fp)
{
    do
    {
        out = fgets(out, MAX_LINE_LENGTH, fp);
    } while (out != NULL && (out[0] == '#' || out[0] == '\n' || out[0] == '\r'));
    return out;
}

/* Gets the index of a given string in a string array
    String must exist in the string array */
int index_of_string(char (*label_array)[MAX_LABEL_LENGTH], int label_array_count, char *label)
{
    int i = 0, found_index = -1;
    for (i = 0; i < label_array_count; i++)
    {
        if (strcmp(label_array[i], label) == 0)
        {
            found_index = i;
            break;
        }
    }
    assert(found_index != -1);
    return found_index;
}

/* Reads a road line from the network file and returns it
    Also add itself to the vertices it is connected to */
road read_road_line(const char *line, const network *net, road *this_ptr, char (*vertex_labels)[MAX_LABEL_LENGTH], char (*road_labels)[MAX_LABEL_LENGTH])
{
    char discard[MAX_LINE_LENGTH],
         vertex_dest_label[MAX_LINE_LENGTH],
         vertex_source_label[MAX_LINE_LENGTH];
    vertex *v_dest, *v_source;
    int road_type, lanes;
    double length;

    /* Get all variables in the line */
    sscanf(line, "%[^{]"  /* discard whatever is scanned. */
                 "{%[^,]" /* vertex in */
                 ",%[^,]" /* vertex out */
                 ",%lf, " /* Length */
                 "%d, "   /* Number of Lanes */
                 "%d}",   /* bus lanes */
           discard,vertex_source_label, vertex_dest_label, &length, &lanes, &road_type);

    /* Assign road to found vertices aswell */ 
    v_dest = net->vertices + index_of_string(vertex_labels, net->vertex_count, vertex_dest_label);
    v_source = net->vertices + index_of_string(vertex_labels, net->vertex_count, vertex_source_label);
    v_dest->roads_in[v_dest->roads_in_count] = this_ptr;
    v_dest->roads_in_count++;
    v_source->roads_out[v_source->roads_out_count] = this_ptr;
    v_source->roads_out_count++;

    return make_road(road_type, v_dest, length, lanes);
}

/* Reads a crossing from the network file and returns it */
vertex read_crossing_line(const char *line, const network *net, int index, char (*vertex_labels)[MAX_LABEL_LENGTH], char (*road_labels)[MAX_LABEL_LENGTH]){
    char label[MAX_LABEL_LENGTH];
    vertex base_v = net->vertices[index];
    int phasecount;
    int phase_times[MAX_PHASES];
    char string_in_roads_phases[MAX_LINE_LENGTH];
    int in_roads_phases[MAX_IN_OUT_ROADS];

    read_label(line, label);

    sscanf(line, "%*[^{]{%d, [%d,%d],[%[^]]]}", &phasecount, &phase_times[0], &phase_times[1], string_in_roads_phases);

    find_in_roads_phases(in_roads_phases, string_in_roads_phases, label, vertex_labels, road_labels, net);
   
    return make_crossing(base_v.roads_in, base_v.roads_in_count, base_v.roads_out, base_v.roads_out_count, phasecount, in_roads_phases, phase_times);
}

void find_in_roads_phases(int *in_roads_phases, const char *string_in_roads_phases, char* v_label, char (*vertex_labels)[MAX_LABEL_LENGTH], char (*road_labels)[MAX_LABEL_LENGTH], const network *net){
    char r_label[MAX_LABEL_LENGTH];
    int phase;
    int element = 0;
    int number_of_in_roads = find_number_of_in_roads(string_in_roads_phases);
    int i, index;
    for (i = 0; i < number_of_in_roads; i++)
    {
        find_road_label(r_label, &phase, &element, string_in_roads_phases);
       
        index = index_of_in_road(
            (net->vertices + index_of_string(vertex_labels, net->vertex_count, v_label)), 
            (net->roads + index_of_string(road_labels, net->road_count, r_label)));

        *(in_roads_phases + index) = phase;
    }
}

int find_number_of_in_roads(const char *string_in_roads_phases){
    int number_of_in_roads = 0;
    int i = 0;
    for (i = 0; i <= strlen(string_in_roads_phases); i++)
    {
        if (string_in_roads_phases[i] == ',')
            number_of_in_roads++;
    }
    return number_of_in_roads;
}

void find_road_label(char *road_label, int *phase, int *element, const char *string_in_roads_phases){
    int i = 0;
    char label[MAX_LABEL_LENGTH];
    memset(label, '\0', sizeof(&label));
    while (string_in_roads_phases[*element] != ',')
    {
        label[i] = string_in_roads_phases[*element];
        i++;
        (*element)++;
    }
    label[i] = '\0';
    sscanf(label,"%[^=]=%d", road_label, phase);
    (*element)++;
}

/* Reads an endpoint line from the network file and returns it */
vertex read_endpoint_line(const char *line, const network *net, int index, char (*vertex_labels)[MAX_LABEL_LENGTH], char (*road_labels)[MAX_LABEL_LENGTH]){
    char label[MAX_LABEL_LENGTH];
    vertex base_v = net->vertices[index];
    double w_in, w_out;
    read_label(line, label);
    sscanf(line, "%*[^{]{"
    "%lf," /* Weight entering point */
    "%lf}" /* Weight going out */
    , &w_in, &w_out);
    base_v = make_endpoint(base_v.roads_in, base_v.roads_in_count, base_v.roads_out, base_v.roads_out_count, w_in, w_out);
    return base_v;
}

/* Reads a busstop line from the network file and returns it */
vertex read_busstop_line(const char *line, const network *net, int index, char (*vertex_labels)[MAX_LABEL_LENGTH], char (*road_labels)[MAX_LABEL_LENGTH]){
    char label[MAX_LABEL_LENGTH];
    vertex base_v = net->vertices[index];
    double people_per_sec;
    read_label(line, label);
    sscanf(line, "%*[^{]{"
    "%lf} " /* People per second */
    , &people_per_sec);
    return make_busstop(base_v.roads_in, base_v.roads_in_count, base_v.roads_out, base_v.roads_out_count, people_per_sec);
}

/* Reads an entire busroute line and returns the busroute */
busroute read_busroute_label(const char *line, const network *net, char (*vertex_labels)[MAX_LABEL_LENGTH]){
    busroute b_busroute;

    b_busroute.b_route = make_the_bus_route(line, net, vertex_labels);

    b_busroute.bus_interval = initialize_bus_interval(line);

    return make_busroute(b_busroute.b_route, b_busroute.bus_interval);
}

/* Creates a route from a busroute line from the network file */
route make_the_bus_route(const char *line, const network *net, char (*vertex_labels)[MAX_LABEL_LENGTH]){
    char vertices_line[100];
    route bus_route;
    bus_route.cur_point = 0;
    bus_route.route_length = count_number_of_vertices(line);
    bus_route.points = (vertex**)malloc(bus_route.route_length * sizeof(vertex*));

    sscanf(line, "%*[^[][ %[^]]", vertices_line);

    find_vertices_on_busroute(bus_route.route_length, vertices_line, net, vertex_labels ,&bus_route);

    return bus_route;
}

/* Returns number of vertices in a bus route */
int count_number_of_vertices(const char *line){
    int i = 0, comma = 0;
    while (line[i] != ']')
    {
        if (line[i] == ',')
            comma++;

        i++;
    }

    return comma;
}

/* Find all vertices in a busroute in the network file */
void find_vertices_on_busroute(const int number_of_vertices, char* vertices_line, const network* net, char(*vertex_labels)[MAX_LABEL_LENGTH], route* busroute){
    int i, element = 0;
    char vertex_string[100];   
    for (i = 0; i < number_of_vertices; i++)
    {
        read_vertex(vertices_line, vertex_string, &element);
        busroute->points[i] = &(net->vertices[index_of_string(vertex_labels, net->vertex_count, vertex_string)]);
        element++;
    }
}

/* Reads a vertex element from list of vertices in network file*/
void read_vertex(const char *vertices_line, char *vertex, int *element){
    int i = 0;
    memset(vertex, '\0', sizeof(&vertex));
    while (vertices_line[*element] != ',')
    {
        vertex[i] = vertices_line[*element];
        i++;
        (*element)++;
    }
    vertex[i] = '\0';
}

/* Reads the bus interval variable from line in network file */
double initialize_bus_interval(const char *line){
    double bus_interval;
    sscanf(line, "%*[^]] ]," 
                    "%lf }", &bus_interval);
    return bus_interval;
}

/* Free all memory used by the network. Currently unused as not fully developed */
void free_network(network *net)
{
    int i;
    free(net->roads);
    free(net->vehicles);
    for (i = 0; i < net->vehicle_count; i++)
    {
        free(net->vehicles[i].cur_route.points);
    }
    free(net->vertices);

    free(net);
}
