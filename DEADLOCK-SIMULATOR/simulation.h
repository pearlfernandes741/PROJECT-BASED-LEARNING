#ifndef SIMULATION_H
#define SIMULATION_H

#include "algorithms.h"

int request_resources(int process, const int request[]);
int release_resources(int process, const int release[]);
int recover_process(int process);

#endif

