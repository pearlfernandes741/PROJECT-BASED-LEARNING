#include <stdio.h>
#include "process.h"

void round_robin(Process p[], int n, int quantum) {
    if (n <= 0 || n > MAX || quantum <= 0) {
        printf("Invalid number of processes or time quantum.\n");
        return;
    }

    int remaining[MAX];
    int first_start[MAX];
    int queue[MAX];
    int front = 0, rear = 0, count = 0;
    int completed = 0, time = 0;
    int added[MAX] = {0};

    double total_waiting = 0;
    double total_turnaround = 0;
    double total_busy = 0;

    for (int i = 0; i < n; i++) {
        if (p[i].arrival_time < 0 || p[i].burst_time <= 0) {
            printf("Invalid arrival time or burst time.\n");
            return;
        }

        remaining[i] = p[i].burst_time;
        first_start[i] = -1;

        p[i].completion_time = 0;
        p[i].waiting_time = 0;
        p[i].turnaround_time = 0;
        p[i].response_time = 0;

        total_busy += p[i].burst_time;
    }

    printf("\nRound Robin Gantt Chart (Quantum = %d):\n", quantum);

    while (completed < n) {

        for (int i = 0; i < n; i++) {
            if (!added[i] && p[i].arrival_time <= time) {
                queue[rear] = i;
                rear = (rear + 1) % MAX;
                count++;
                added[i] = 1;
            }
        }

        if (count == 0) {
            printf("| Idle ");
            time++;
            continue;
        }

        int current = queue[front];
        front = (front + 1) % MAX;
        count--;

        if (first_start[current] == -1)
            first_start[current] = time;

        int slice = remaining[current] < quantum
                    ? remaining[current] : quantum;

        printf("| P%d ", p[current].pid);

        for (int t = 0; t < slice; t++) {
            time++;
            remaining[current]--;

            for (int i = 0; i < n; i++) {
                if (!added[i] && p[i].arrival_time <= time) {
                    queue[rear] = i;
                    rear = (rear + 1) % MAX;
                    count++;
                    added[i] = 1;
                }
            }
        }

        if (remaining[current] == 0) {
            p[current].completion_time = time;

            p[current].turnaround_time =
                time - p[current].arrival_time;

            p[current].waiting_time =
                p[current].turnaround_time - p[current].burst_time;

            p[current].response_time =
                first_start[current] - p[current].arrival_time;

            completed++;
        } else {
            queue[rear] = current;
            rear = (rear + 1) % MAX;
            count++;
        }
    }

    printf("|\nFinal time: %d\n", time);

    printf("\nPID\tAT\tBT\tCT\tTAT\tWT\tRT\n");

    for (int i = 0; i < n; i++) {
        printf("P%d\t%d\t%d\t%d\t%d\t%d\t%d\n",
               p[i].pid,
               p[i].arrival_time,
               p[i].burst_time,
               p[i].completion_time,
               p[i].turnaround_time,
               p[i].waiting_time,
               p[i].response_time);

        total_waiting += p[i].waiting_time;
        total_turnaround += p[i].turnaround_time;
    }

    printf("Average Waiting Time: %.2f\n",
           total_waiting / n);

    printf("Average Turnaround Time: %.2f\n",
           total_turnaround / n);

    printf("CPU Utilization: %.2f%%\n",
           time > 0 ? total_busy * 100.0 / time : 0.0);
}