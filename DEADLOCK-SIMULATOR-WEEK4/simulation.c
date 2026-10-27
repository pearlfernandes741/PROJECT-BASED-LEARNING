#include "simulation.h"

int request_resources(int process, const int request[])
{
    if (process < 0 || process >= process_count)
        return -1;

    if (!calculate_need())
        return -1;

    for (int j = 0; j < resource_count; j++) {
        if (request[j] < 0 ||
            request[j] > need[process][j] ||
            request[j] > available[j])
            return 0;
    }

    int old_available[MAX_RESOURCES];
    int old_allocation[MAX_RESOURCES];
    int old_need[MAX_RESOURCES];

    for (int j = 0; j < resource_count; j++) {
        old_available[j] = available[j];
        old_allocation[j] = allocation[process][j];
        old_need[j] = need[process][j];

        available[j] -= request[j];
        allocation[process][j] += request[j];
        need[process][j] -= request[j];
    }

    int sequence[MAX_PROCESSES];

    if (!banker_algorithm(sequence)) {
        for (int j = 0; j < resource_count; j++) {
            available[j] = old_available[j];
            allocation[process][j] = old_allocation[j];
            need[process][j] = old_need[j];
        }
        return 0;
    }

    return 1;
}

int release_resources(int process, const int release[])
{
    if (process < 0 || process >= process_count)
        return -1;

    for (int j = 0; j < resource_count; j++) {
        if (release[j] < 0 ||
            release[j] > allocation[process][j])
            return 0;
    }

    for (int j = 0; j < resource_count; j++) {
        allocation[process][j] -= release[j];
        available[j] += release[j];
    }

    return calculate_need();
}

int recover_process(int process)
{
    if (process < 0 || process >= process_count)
        return -1;

    for (int j = 0; j < resource_count; j++) {
        available[j] += allocation[process][j];
        allocation[process][j] = 0;
        maximum[process][j] = 0;
        need[process][j] = 0;
    }

    return 1;
}

