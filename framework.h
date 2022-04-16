#define DATA_COLUMNS 16
#define DATA_ROWS 1048576/8

void sim_loop(int(*step_function)(int iteration, double delta, double time, void* simulation_data), double deltatime, void *sim_data);
void print_run_check(double time, int iteration);
void sim_end(void);
void sim_init(void);
void sim_log_data(int column, double value);
void set_data_name(int column, const char *name);
void set_data_columns(const char *columns[DATA_COLUMNS]);

#define SIM_STOP 0
#define SIM_CONTINUE 1

