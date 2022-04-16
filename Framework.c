/*
 * Simulation framework for our traffic simulator 
 * Made by SW1A402 in P1 year 2020
 * 
 * Purpose: Easily initialize, update and end the simulation through
 *  sim_init, sim_loop and sim_end
 * 
 * Also enables easy data logging, without having to worry about arrays from the caller,
 *  exports the data to a csv file
 * 
 * Also prints simulation time to console to let the user know how the simulation is going
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "framework.h"

#define PRINT_ITERATION_UPDATE 1000
#define COLUMN_NAME_MAX_LENGTH 100

double data[DATA_COLUMNS][DATA_ROWS];
int cur_data_index[DATA_COLUMNS];
char column_name[DATA_COLUMNS][COLUMN_NAME_MAX_LENGTH];
void export_data_to_csv(char* filename);
void print_data(void);
void initialize_data(void);

/* Logs data value at the next open row for a given column */
void sim_log_data(int column, double value){
    data[column][cur_data_index[column]] = value;
    cur_data_index[column]++;

    if (cur_data_index[column] >= DATA_ROWS){
        printf("Data out of bounds error, log less data or increase row count.");
        exit(EXIT_FAILURE);
    }
}

/* Sets the column name for given column */
void set_data_name(int column, const char *name){
    strcpy(column_name[column], name);
}

/* Sets the all column name for all columns with string array */
void set_data_columns(const char *columns[DATA_COLUMNS]){
    int i;
    for (i = 0; i < DATA_COLUMNS; i++)
    {
        set_data_name(i, columns[i]);
    }
}

/* All steps to initialize the simulation */
void sim_init(void){
    initialize_data();
}

/* Set all data to 0 in the array */
void initialize_data(void){
    int i, j;
    for (i = 0; i < DATA_COLUMNS; i++)
    {
        cur_data_index[i] = 0;
        for (j = 0; j < DATA_ROWS; j++)
        {
            data[i][j] = 0;
        }
    }
}

/* Run a simulation step function in a loop, and passes all time values and data to step function */
void sim_loop(int(*step_function)(int iteration, double delta, double time, void *simulation_data), 
    double deltatime, void *sim_data){
    int iteration = 0, sim_running = 1;
    double time;
    while (sim_running)
    {
        time = iteration * deltatime;
        sim_running = step_function(iteration, deltatime, iteration * deltatime, sim_data);
        print_run_check(time, iteration);
        iteration++;
    }
}


/* Prints runtime and ... to user to let the user know it is still running, and gives an idea of how fast */
void print_run_check(double time, int iteration){
    if (iteration % PRINT_ITERATION_UPDATE == 0){
        printf("\nSim time: %.0lf", time);
    }
    else if(iteration % (PRINT_ITERATION_UPDATE/10) == 0){
        printf(".");
    }
}

/* All steps to initialize the simulation */
void sim_end(void){
    export_data_to_csv("output.csv");
    printf("\n\nSim complete!\n\n");
    /*print_data();*/    
}

/* Prints data to stdout (unused, but useful for testing) */
void print_data(void){
    int i, j;
    printf("\n\n");
    for (i = 0; i < DATA_COLUMNS; i++)
    {
        printf("_____________________________\n");

        printf("%s: ", column_name[i]);
        for (j = 0; j < cur_data_index[i]; j++)
        {
            printf("%7.2lf, ", data[i][j]);
        }
        printf("\n");
    }
}

/* Saves all data to a CSV file seperated by commas and line breaks */
void export_data_to_csv(char* filename){
    int row, column;
    FILE *fp = fopen(filename, "w");
    if (fp == NULL){
        printf("\n--------------ERROR--------------\n Failed to open file %s for writing!\nIs it in use by something else?\n", filename);
        return;
    }
    for (column = 0; column < DATA_COLUMNS; column++)
    {
        fprintf(fp, "%s", column_name[column]);
        if (column != DATA_COLUMNS - 1){
            fprintf(fp, ",");
        }
        else{
            fprintf(fp, "\n");
        }
    }    
    for (row = 0; row < DATA_ROWS; row++)
    {
        for (column = 0; column < DATA_COLUMNS; column++)
        {
            if (row < cur_data_index[column]){
                fprintf(fp, "%lf", data[column][row]);
            }
            if (column != DATA_COLUMNS - 1){
                fprintf(fp, ",");
            }
            else{
                fprintf(fp, "\n");
            }
        }
    }
    fclose(fp);
}
