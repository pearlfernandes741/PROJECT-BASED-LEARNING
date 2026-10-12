#include <stdio.h>
#include "process.h"

void round_robin(Process p[], int n, int quantum);

int main() {
    Process p[3] = {
        {.pid = 1, .arrival_time = 0, .burst_time = 6, .priority = 1},
        {.pid = 2, .arrival_time = 0, .burst_time = 4, .priority = 2},
        {.pid = 3, .arrival_time = 0, .burst_time = 2, .priority = 3}
    };

    int n = 3;
    int quantum = 3;

    round_robin(p, n, quantum);

    return 0;
}