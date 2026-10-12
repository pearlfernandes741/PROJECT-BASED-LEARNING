#include <stdio.h>
#include "../process.h"

static void print_results(Process p[], int n, int total_time) {
    double total_waiting = 0, total_turnaround = 0, total_busy = 0;

    printf("\nPID\tAT\tBT\tCT\tTAT\tWT\tRT\n");
    for (int i = 0; i < n; i++) {
        printf("P%d\t%d\t%d\t%d\t%d\t%d\t%d\n",
               p[i].pid, p[i].arrival_time, p[i].burst_time,
               p[i].completion_time, p[i].turnaround_time,
               p[i].waiting_time, p[i].response_time);
        total_waiting += p[i].waiting_time;
        total_turnaround += p[i].turnaround_time;
        total_busy += p[i].burst_time;
    }

    printf("Average Waiting Time: %.2f\n", total_waiting / n);
    printf("Average Turnaround Time: %.2f\n", total_turnaround / n);
    printf("CPU Utilization: %.2f%%\n",
           total_time > 0 ? total_busy * 100.0 / total_time : 0.0);
}

void fcfs(Process p[], int n) {
    int count = 0, time = 0;
    int done[MAX] = {0};

    printf("\nFCFS Gantt Chart:\n");

    while (count < n) {
        int selected = -1;

        for (int i = 0; i < n; i++) {
            if (!done[i] && p[i].arrival_time <= time &&
                (selected == -1 ||
                 p[i].arrival_time < p[selected].arrival_time ||
                 (p[i].arrival_time == p[selected].arrival_time &&
                  p[i].pid < p[selected].pid))) {
                selected = i;
            }
        }

        if (selected == -1) {
            printf("| Idle ");
            time++;
            continue;
        }

        printf("| P%d ", p[selected].pid);
        p[selected].response_time = time - p[selected].arrival_time;
        p[selected].waiting_time = time - p[selected].arrival_time;
        time += p[selected].burst_time;
        p[selected].completion_time = time;
        p[selected].turnaround_time = time - p[selected].arrival_time;
        done[selected] = 1;
        count++;
    }

    printf("|\n");
    printf("Final time: %d\n", time);
    print_results(p, n, time);
}

void sjf_non_preemptive(Process p[], int n) {
    int count = 0, time = 0;
    int done[MAX] = {0};

    printf("\nSJF Non-Preemptive Gantt Chart:\n");

    while (count < n) {
        int selected = -1;

        for (int i = 0; i < n; i++) {
            if (!done[i] && p[i].arrival_time <= time &&
                (selected == -1 ||
                 p[i].burst_time < p[selected].burst_time ||
                 (p[i].burst_time == p[selected].burst_time &&
                  p[i].arrival_time < p[selected].arrival_time) ||
                 (p[i].burst_time == p[selected].burst_time &&
                  p[i].arrival_time == p[selected].arrival_time &&
                  p[i].pid < p[selected].pid))) {
                selected = i;
            }
        }

        if (selected == -1) {
            printf("| Idle ");
            time++;
            continue;
        }

        printf("| P%d ", p[selected].pid);
        p[selected].response_time = time - p[selected].arrival_time;
        p[selected].waiting_time = time - p[selected].arrival_time;
        time += p[selected].burst_time;
        p[selected].completion_time = time;
        p[selected].turnaround_time = time - p[selected].arrival_time;
        done[selected] = 1;
        count++;
    }

    printf("|\n");
    printf("Final time: %d\n", time);
    print_results(p, n, time);
}
#include <stdio.h>
#include "../process.h"

