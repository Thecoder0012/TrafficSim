# P1-SW1A402

P1 projekt for gruppe A402 på Software
Denne github blev lavet den 18/11 2020

# Style Guide

Indentation is 4 spaces.
Code should always be indented

Committed comments should always be these /\* \*/

Keep all variables local in scope, and pass through parameters, unless there is a very good reason and is discussed with others.

ifs and else ifs etc are written

```
if (...){
  /* Do something */
}
```

Loops, like for loops, while loops are written

```
while (...)
{
  /* Do something */
}

```

Functions are all lowercase with underscores seperating. Should be descriptive, should be written:

```
void my_function(...){
  /* Do something */
}
```

Variables are all lowercase, with underscores seperating. Should be descriptive. NOT letter variables (l, W, etc) unless only used in small function. Variables can be written:

```
int my_int1, my_integer2;
```

Define konstanter skrives ALL_CAPS_MED_UNDERSCORE

```
#define CONSTANT_VALUE 2
```

# Glossary

## Traffic simulation

A "bus" and a "car" are both structs, that have a "vehicle" struct inside them
The "vehicle" struct holds all variables in common for all vehicles in the simulation

### vehicle

vehicle can be shortened to vhcl

max_acceleration is the acceleration a vehicle has when smashing the brakes
  -a constant

desired_time_headway is the amount of time a car wants to have to the vehicle in front of it
  -a constant

desired_time_lag is the amount of time a car wants to have to the car behind when changing lanes
  -a constant

length is the physical length of the vehicle
  -a constant

speed is the current speed of the vehicle in the simulation
  -a variable

front_vehicle is a pointer to the car in front of it in the current road. If there are no cars in front of it on the current road, this is a NULL pointer
  -a variable



## Framework
