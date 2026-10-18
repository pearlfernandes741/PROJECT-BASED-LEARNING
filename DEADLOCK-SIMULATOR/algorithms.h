#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#define MAX_PROCESSES 10
#define MAX_RESOURCES 10

extern int process_count;
extern int resource_count;

extern int allocation[MAX_PROCESSES][MAX_RESOURCES];
extern int maximum[MAX_PROCESSES][MAX_RESOURCES];
extern int need[MAX_PROCESSES][MAX_RESOURCES];
extern int available[MAX_RESOURCES];

int calculate_need(void);
int banker_algorithm(int safe_sequence[]);
int detect_deadlock(int deadlocked[]);

#endif

