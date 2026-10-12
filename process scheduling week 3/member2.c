#include <stdio.h>
#include "../process.h"

static void show_results(Process p[], int n, int total_time) {
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

static void run_preemptive(Process p[], int n, int use_priority) {
int remaining[MAX], first_start[MAX];
int completed = 0, time = 0;

for (int i = 0; i < n; i++) {
    remaining[i] = p[i].burst_time;
    first_start[i] = -1;
}

printf("\nGantt Chart:\n");

while (completed < n) {
    int selected = -1;

    for (int i = 0; i < n; i++) {
        if (remaining[i] <= 0 || p[i].arrival_time > time)
            continue;

        if (selected == -1) {
            selected = i;
            continue;
        }

        int better;
        if (use_priority) {
            better = p[i].priority < p[selected].priority;
        } else {
            better = remaining[i] < remaining[selected];
        }

        if (better ||
            ((!use_priority ? remaining[i] == remaining[selected]
                            : p[i].priority == p[selected].priority) &&
             (p[i].arrival_time < p[selected].arrival_time ||
              (p[i].arrival_time == p[selected].arrival_time &&
               p[i].pid < p[selected].pid)))) {
            selected = i;
        }
    }

    if (selected == -1) {
        printf("| Idle ");
        time++;
        continue;
    }

    if (first_start[selected] == -1)
        first_start[selected] = time;

    printf("| P%d ", p[selected].pid);
    remaining[selected]--;
    time++;

    if (remaining[selected] == 0) {
        p[selected].completion_time = time;
        p[selected].turnaround_time =
            time - p[selected].arrival_time;
        p[selected].waiting_time =
            p[selected].turnaround_time - p[selected].burst_time;
        p[selected].response_time =
            first_start[selected] - p[selected].arrival_time;
        completed++;
    }
}

printf("|\nFinal time: %d\n", time);
show_results(p, n, time);

}

void sjf_preemptive(Process p[], int n) {
printf("\nSJF Preemptive (Shortest Remaining Time First)\n");
run_preemptive(p, n, 0);
}

void priority_non_preemptive(Process p[], int n) {
int done[MAX] = {0};
int completed = 0, time = 0;

printf("\nPriority Non-Preemptive Gantt Chart:\n");

while (completed < n) {
    int selected = -1;

    for (int i = 0; i < n; i++) {
        if (done[i] || p[i].arrival_time > time)
            continue;

        if (selected == -1 ||
            p[i].priority < p[selected].priority ||
            (p[i].priority == p[selected].priority &&
             (p[i].arrival_time < p[selected].arrival_time ||
              (p[i].arrival_time == p[selected].arrival_time &&
               p[i].pid < p[selected].pid)))) {
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
    completed++;
}

printf("|\nFinal time: %d\n", time);
show_results(p, n, time);

}

void priority_preemptive(Process p[], int n) {
printf("\nPriority Preemptive\n");
run_preemptive(p, n, 1);
}