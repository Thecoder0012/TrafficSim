traffic_sim: traffic_sim.c
	gcc framework.c -ansi -pedantic -Wall \
	traffic_sim.c -ansi -pedantic -Wall \
	vehicle_lists.c -ansi -pedantic -Wall \
	network_definitions.c -ansi -pedantic -Wall \
	vehicle_behavior.c -ansi -pedantic -Wall \
	network_reader.c -ansi -pedantic -Wall \
	Bus_behaviour.c -ansi -pedantic -Wall \
	add_remove_vehicle.c -ansi -pedantic -Wall \
	spawn_cars.c -ansi -pedantic -Wall \
	Traffic_sim_manager.c -ansi -pedantic -Wall \
	crossing.c -I -ansi -pedantic -Wall
