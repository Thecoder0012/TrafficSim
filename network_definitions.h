/* Defines all structs relating to the traffic network, and prototypes for make functions */

/* Define guard, avoids multiple definitions */
#ifndef NETWORK_DEFINITIONS
#define NETWORK_DEFINITIONS 

#define MAX_IN_OUT_ROADS 8
#define MAX_PHASES 8

typedef enum road_type      {road_any, road_bus}                road_type;
typedef enum vehicle_type   {vhcl_car, vhcl_bus}                vehicle_type;
typedef enum vertex_type    {v_crossing, v_endpoint, v_busstop} vertex_type;
typedef enum activity       {active_at_start, active, inactive, waiting} activity;

typedef enum goal {free_flow, follow_car, red_light, change_lane, bus_stop, panic_wait} goal;

/* Typedef all structs before, as this allows structs to cross refer to eachother */
typedef struct road     road;
typedef struct vehicle  vehicle;
typedef struct vertex   vertex;
typedef struct crossing crossing;
typedef struct busstop  busstop;
typedef struct endpoint endpoint;
typedef struct network  network;
typedef struct route    route;
typedef struct busroute busroute;

/* Defines a route, and current position along it */
struct route{
    int bus_id;       /* The busroute id, only used if a bus */
    int cur_point;    /* Index of current point */
    int route_length; /* Number of points in route */
    vertex **points;   /* Vertices on route */
};

/* Defines a generic vehicle */
struct vehicle{
    vehicle_type type;              /* Car or bus */
    double max_acceleration;        /* Maximum accel */
    double desired_time_headway;    /* the time distance they want to reach */
    double desired_time_lag;        /* Desired distance backwards for lane changing */
    double length;                  /* Length of car. Meters behind front */
    double speed;                   /* Current speed */
    double position;                /* Position along current road */
    double distance_to_goal;        /* The distance until goal set by cur_goal */
    double speed_of_goal;           /* The speed of goal set by cur_goal */
    goal cur_goal;                  /* What the vehicle is trying to reach/follow/do */
    road *cur_road;                 /* Road the car is on */
    vehicle *front_vehicle;         /* The vehicle in front of it on current road */
    route cur_route;                /* The route it follows */
    activity cur_activity;          /* The current state of the vehicle (e.g. inactive) */
    double spawn_time;              /* The time of the car spawning, used for data logging */
};

/* A road that can used by vehicles. Through the last_in_vhcl pointer, 
    has access to all vehicles through a linked list */
struct road{
    road_type type; /* Bus road or not, road_any road_bus */
    vertex *destination;    /* The vertex it leads to */
    double length;  /* Length in meters */
    int lanes;      /* Number of lanes (UNUSED) */

    int vhcl_count;         /* Number of vehicles on the road */
    vehicle *last_in_vhcl;  /* the backmost vehicle. First element in linked list  */
};


/* Specific data used for traffic lights */
struct crossing{
    int phasecount;                         /* The amount of light phases that exist */
    int phase_times[MAX_PHASES];            /* The time that a phase at a given index lasts */
    int in_roads_phases[MAX_IN_OUT_ROADS];  /* What light phase a specific road belongs to */
    int out_road_open[MAX_IN_OUT_ROADS];    /* What roads out cannot be used currently */
    int current_phase;                      /* The current light phase */
    double cur_timer;                       /* Timer for changing light phase */
};

/* Specific data used for busstops */
struct busstop{
    vehicle *bus;               /* Pointer to current held bus */
    double cur_waiting;         /* Whether a bus is currently waiting in the stop */
    double people_per_sec;      /* The amount of people arriving each second */
    double cur_waiting_time;    /* Timer until bus exits */
    int bus_coming_out_bool;    /* 0: bus not coming out 1: bus coming out */
    int a_bus_at_the_stop;      /* 0: no buses at the stop  1: yes a bus at the stop */
};

/* Specific data used for endpoints, where vehicles can enter or exit the simulation */
struct endpoint{
    double weight_in;   /* How much the point is used to enter the simulation */
    double weight_out;  /* How much the point is used to exit the simulation */
};

/* Generic vertex with roads and type */
struct vertex{
    road* roads_in[MAX_IN_OUT_ROADS];       /* Road that go into the vertex */
    int roads_in_count;                     /* Amount of roads */
    road* roads_out[MAX_IN_OUT_ROADS];      /* Road that go into the vertex */
    int roads_out_count;                    /* Amount of roads */
    vertex_type vertex_type;                /* Type of current vertex */
    union{ /* union that keeps specific information about the given vertex type */
        crossing cross;
        busstop stop;
        endpoint end;
    } type;
};

struct network{
    vehicle *vehicles;      /* All non-bus vehicles, active or inactive, in the simulation */
    int vehicle_count; 
    road *roads;            /* All roads in the simulation */
    int road_count;
    vertex *vertices;       /* All vertices in the simulation */
    int vertex_count;
    busroute *busroutes;    /* All busroutes in the simulation */
    int busroute_count;
    vehicle *buses;         /* All buses, active or inactive, in the simulation */
    int bus_count;
    int car_spawn_period;   /* The period that cars will spawn over */
};

/* A busroute. Is just a route, with added interval time and time keeping */
struct busroute{
    route b_route;  
    double bus_interval;
    double timer;
};

/* Function prototypes for making parts of the network */
vehicle make_vehicle(vehicle_type type, double max_acceleration, double desired_time_headway, double desired_time_lag, double length, route *cur_route);
vehicle make_bus(vehicle_type type, double max_acceleration, double desired_time_headway, double desired_time_lag, double length);
vertex make_base_vertex(road *roads_in[MAX_IN_OUT_ROADS], int roads_in_count, road *roads_out[MAX_IN_OUT_ROADS], int roads_out_count);
vertex make_crossing(road *roads_in[MAX_IN_OUT_ROADS], int roads_in_count, road *roads_out[MAX_IN_OUT_ROADS], int roads_out_count, int phasecount, int in_roads_phases[MAX_IN_OUT_ROADS], int *phase_times);
vertex make_endpoint(road *roads_in[MAX_IN_OUT_ROADS], int roads_in_count, road *roads_out[MAX_IN_OUT_ROADS], int roads_out_count, int weight_in, int weight_out);
vertex make_busstop(road *roads_in[MAX_IN_OUT_ROADS], int roads_in_count, road *roads_out[MAX_IN_OUT_ROADS], int roads_out_count, double people_per_second);
road make_road(road_type type, vertex *destination, double length, int lanes);
route make_route(vertex **points, int route_length);
busroute make_busroute(route the_route, double interval);
network make_network(road *roads, int road_count, vehicle *vehicles, int vehicle_count, vertex *vertices, int vertex_count, int car_spawn_period);

#endif
