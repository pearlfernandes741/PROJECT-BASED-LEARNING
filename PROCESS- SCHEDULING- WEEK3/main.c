
#include <stdio.h>
#include <string.h>
#include "process.h"

void fcfs(Process p[], int n);
void sjf_non_preemptive(Process p[], int n);
void sjf_preemptive(Process p[], int n);
void priority_non_preemptive(Process p[], int n);
void priority_preemptive(Process p[], int n);
void round_robin(Process p[], int n, int quantum);

int main(void) {
    Process original[MAX], work[MAX];
    int n, choice, quantum;

    printf("====================================\n");
    printf("   PROCESS SCHEDULING SIMULATOR\n");
    printf("====================================\n");

    printf("Enter number of processes (1-%d): ", MAX);
    if (scanf("%d", &n) != 1 || n < 1 || n > MAX) {
        printf("Invalid number of processes.\n");
        return 1;
    }

    for (int i = 0; i < n; i++) {
        original[i].pid = i + 1;

        printf("\nProcess P%d\n", original[i].pid);
        printf("Arrival Time: ");
        if (scanf("%d", &original[i].arrival_time) != 1 ||
            original[i].arrival_time < 0) {
            printf("Invalid arrival time.\n");
            return 1;
        }

        printf("Burst Time: ");
        if (scanf("%d", &original[i].burst_time) != 1 ||
            original[i].burst_time <= 0) {
            printf("Invalid burst time.\n");
            return 1;
        }

        printf("Priority (smaller number = higher priority): ");
        if (scanf("%d", &original[i].priority) != 1) {
            printf("Invalid priority.\n");
            return 1;
        }

        original[i].waiting_time = 0;
        original[i].turnaround_time = 0;
        original[i].response_time = 0;
        original[i].completion_time = 0;
    }

    do {
        printf("\n========== SCHEDULING MENU ==========\n");
        printf("1. FCFS\n");
        printf("2. SJF (Non-Preemptive)\n");
        printf("3. SJF (Preemptive)\n");
        printf("4. Priority (Non-Preemptive)\n");
        printf("5. Priority (Preemptive)\n");
        printf("6. Round Robin\n");
        printf("7. Exit\n");
        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input.\n");
            return 1;
        }

        if (choice >= 1 && choice <= 6) {
            memcpy(work, original, n * sizeof(Process));
        }

        switch (choice) {
            case 1:
                fcfs(work, n);
                break;
            case 2:
                sjf_non_preemptive(work, n);
                break;
            case 3:
                sjf_preemptive(work, n);
                break;
            case 4:
                priority_non_preemptive(work, n);
                break;
            case 5:
                priority_preemptive(work, n);
                break;
            case 6:
                printf("Enter time quantum: ");
                if (scanf("%d", &quantum) != 1 || quantum <= 0) {
                    printf("Time quantum must be positive.\n");
                    return 1;
                }
                round_robin(work, n, quantum);
                break;
            case 7:
                printf("Exiting simulator.\n");
                break;
            default:
                printf("Invalid choice. Select 1 to 7.\n");
        }
    } while (choice != 7);

    return 0;
}
