#ifndef PROCESS_H
#define PROCESS_H

#define MAX 100

typedef struct {
    int pid;
    int arrival_time;
    int burst_time;
    int priority;
    int waiting_time;
    int turnaround_time;
    int response_time;
    int completion_time;
} Process;

#endif
