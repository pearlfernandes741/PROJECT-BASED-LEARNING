
#include <stdio.h>
#include "algorithms.h"

int process_count = 5;
int resource_count = 3;

int allocation[MAX_PROCESSES][MAX_RESOURCES] = {
    {0, 1, 0},
    {2, 0, 0},
    {3, 0, 2},
    {2, 1, 1},
    {0, 0, 2}
};

int maximum[MAX_PROCESSES][MAX_RESOURCES] = {
    {7, 5, 3},
    {3, 2, 2},
    {9, 0, 2},
    {2, 2, 2},
    {4, 3, 3}
};

int need[MAX_PROCESSES][MAX_RESOURCES];
int available[MAX_RESOURCES] = {3, 3, 2};

int calculate_need(void)
{
    for (int i = 0; i < process_count; i++) {
        for (int j = 0; j < resource_count; j++) {
            if (maximum[i][j] < allocation[i][j]) {
                return 0;
            }

            need[i][j] = maximum[i][j] - allocation[i][j];
        }
    }

    return 1;
}

int banker_algorithm(int safe_sequence[])
{
    int work[MAX_RESOURCES];
    int finish[MAX_PROCESSES] = {0};
    int count = 0;

    for (int j = 0; j < resource_count; j++) {
        work[j] = available[j];
    }

    if (!calculate_need()) {
        return 0;
    }

    while (count < process_count) {
        int found = 0;

        for (int i = 0; i < process_count; i++) {
            if (finish[i]) {
                continue;
            }

            int possible = 1;

            for (int j = 0; j < resource_count; j++) {
                if (need[i][j] > work[j]) {
                    possible = 0;
                    break;
                }
            }

            if (possible) {
                for (int j = 0; j < resource_count; j++) {
                    work[j] += allocation[i][j];
                }

                safe_sequence[count++] = i;
                finish[i] = 1;
                found = 1;
            }
        }

        if (!found) {
            return 0;
        }
    }

    return 1;
}

int detect_deadlock(int deadlocked[])
{
    int work[MAX_RESOURCES];
    int finish[MAX_PROCESSES] = {0};
    int count = 0;

    for (int j = 0; j < resource_count; j++) {
        work[j] = available[j];
    }

    if (!calculate_need()) {
        for (int i = 0; i < process_count; i++) {
            deadlocked[i] = i;
        }
        return process_count;
    }

    for (int i = 0; i < process_count; i++) {
        int has_allocation = 0;

        for (int j = 0; j < resource_count; j++) {
            if (allocation[i][j] > 0) {
                has_allocation = 1;
                break;
            }
        }

        finish[i] = !has_allocation;
    }

    int changed;

    do {
        changed = 0;

        for (int i = 0; i < process_count; i++) {
            if (finish[i]) {
                continue;
            }

            int possible = 1;

            for (int j = 0; j < resource_count; j++) {
                if (need[i][j] > work[j]) {
                    possible = 0;
                    break;
                }
            }

            if (possible) {
                for (int j = 0; j < resource_count; j++) {
                    work[j] += allocation[i][j];
                }

                finish[i] = 1;
                changed = 1;
            }
        }
    } while (changed);

    for (int i = 0; i < process_count; i++) {
        if (!finish[i]) {
            deadlocked[count++] = i;
        }
    }

    return count;
}
