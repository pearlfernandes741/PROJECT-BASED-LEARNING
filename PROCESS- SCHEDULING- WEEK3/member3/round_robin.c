// Include standard input/output functions like printf
#include <stdio.h>

// Include the custom header containing Process structure and MAX
#include "process.h"

// Define the Round Robin scheduling function
// p[] stores processes, n is the number of processes
// quantum is the maximum CPU time given to each process per turn
void round_robin(Process p[], int n, int quantum) {

    // Check whether the number of processes and quantum are valid
    if (n <= 0 || n > MAX || quantum <= 0) {

        // Display an error message for invalid input
        printf("Invalid number of processes or time quantum.\n");

        // Stop executing the function
        return;
    }

    // Store the remaining burst time of each process
    int remaining[MAX];

    // Store the first CPU start time of each process
    int first_start[MAX];

    // Create an array to implement the ready queue
    int queue[MAX];

    // Initialize queue front, rear, and number of queued processes
    int front = 0, rear = 0, count = 0;

    // Track completed processes and current simulation time
    int completed = 0, time = 0;

    // Track whether each process has entered the ready queue
    int added[MAX] = {0};

    // Store the total waiting time of all processes
    double total_waiting = 0;

    // Store the total turnaround time of all processes
    double total_turnaround = 0;

    // Store the total CPU burst time of all processes
    double total_busy = 0;

    // Initialize scheduling information for every process
    for (int i = 0; i < n; i++) {

        // Check for invalid arrival time or burst time
        if (p[i].arrival_time < 0 || p[i].burst_time <= 0) {

            // Display an error message
            printf("Invalid arrival time or burst time.\n");

            // Stop the function
            return;
        }

        // Initially, remaining time equals the original burst time
        remaining[i] = p[i].burst_time;

        // Set -1 to indicate that the process has not started
        first_start[i] = -1;

        // Initialize completion time to zero
        p[i].completion_time = 0;

        // Initialize waiting time to zero
        p[i].waiting_time = 0;

        // Initialize turnaround time to zero
        p[i].turnaround_time = 0;

        // Initialize response time to zero
        p[i].response_time = 0;

        // Add the process burst time to total CPU busy time
        total_busy += p[i].burst_time;
    }

    // Display the heading of the Round Robin Gantt chart
    printf("\nRound Robin Gantt Chart (Quantum = %d):\n", quantum);

    // Continue scheduling until every process is completed
    while (completed < n) {

        // Check all processes for arrival at the current time
        for (int i = 0; i < n; i++) {

            // Check whether a process has arrived and is not yet added
            if (!added[i] && p[i].arrival_time <= time) {

                // Add the process index to the ready queue
                queue[rear] = i;

                // Move rear to the next position using circular indexing
                rear = (rear + 1) % MAX;

                // Increase the number of processes in the queue
                count++;

                // Mark the process as already added
                added[i] = 1;
            }
        }

        // Check whether the ready queue is empty
        if (count == 0) {

            // Display idle CPU time in the Gantt chart
            printf("| Idle ");

            // Increase simulation time by one unit
            time++;

            // Skip the remaining statements and repeat the loop
            continue;
        }

        // Select the process at the front of the ready queue
        int current = queue[front];

        // Move front to the next circular queue position
        front = (front + 1) % MAX;

        // Decrease queue count after removing the process
        count--;

        // Check whether this process is running for the first time
        if (first_start[current] == -1)

            // Record the time when the process first gets the CPU
            first_start[current] = time;

        // Choose the smaller value between remaining time and quantum
        // This prevents running a process longer than it needs
        int slice = remaining[current] < quantum
                    ? remaining[current] : quantum;

        // Display the selected process in the Gantt chart
        printf("| P%d ", p[current].pid);

        // Execute the process for its allocated time slice
        for (int t = 0; t < slice; t++) {

            // Increase the current simulation time
            time++;

            // Reduce the remaining burst time by one unit
            remaining[current]--;

            // Check whether new processes have arrived
            for (int i = 0; i < n; i++) {

                // Check whether a new process has arrived
                if (!added[i] && p[i].arrival_time <= time) {

                    // Add the newly arrived process to the queue
                    queue[rear] = i;

                    // Move rear circularly to the next position
                    rear = (rear + 1) % MAX;

                    // Increase the ready queue count
                    count++;

                    // Mark the process as added to the queue
                    added[i] = 1;
                }
            }
        }

        // Check whether the current process has finished execution
        if (remaining[current] == 0) {

            // Record the time when the process completes
            p[current].completion_time = time;

            // Calculate turnaround time
            // Turnaround Time = Completion Time - Arrival Time
            p[current].turnaround_time =
                time - p[current].arrival_time;

            // Calculate waiting time
            // Waiting Time = Turnaround Time - Burst Time
            p[current].waiting_time =
                p[current].turnaround_time - p[current].burst_time;

            // Calculate response time
            // Response Time = First Start Time - Arrival Time
            p[current].response_time =
                first_start[current] - p[current].arrival_time;

            // Increase the number of completed processes
            completed++;

        } else {

            // Add the unfinished process back to the ready queue
            queue[rear] = current;

            // Move rear to the next circular position
            rear = (rear + 1) % MAX;

            // Increase the ready queue count
            count++;
        }
    }

    // Display the end of the Gantt chart and final simulation time
    printf("|\nFinal time: %d\n", time);

    // Display column headings for process scheduling results
    // AT = Arrival Time, BT = Burst Time, CT = Completion Time
    // TAT = Turnaround Time, WT = Waiting Time, RT = Response Time
    printf("\nPID\tAT\tBT\tCT\tTAT\tWT\tRT\n");

    // Display scheduling results for every process
    for (int i = 0; i < n; i++) {

        // Print the process ID and calculated scheduling times
        printf("P%d\t%d\t%d\t%d\t%d\t%d\t%d\n",
               p[i].pid,
               p[i].arrival_time,
               p[i].burst_time,
               p[i].completion_time,
               p[i].turnaround_time,
               p[i].waiting_time,
               p[i].response_time);

        // Add this process's waiting time to the total
        total_waiting += p[i].waiting_time;

        // Add this process's turnaround time to the total
        total_turnaround += p[i].turnaround_time;
    }

    // Calculate and display average waiting time
    printf("Average Waiting Time: %.2f\n",
           total_waiting / n);

    // Calculate and display average turnaround time
    printf("Average Turnaround Time: %.2f\n",
           total_turnaround / n);

    // Calculate and display CPU utilization as a percentage
    // CPU Utilization = Total Busy Time / Total Simulation Time * 100
    // If total time is zero, display zero to avoid division by zero
    printf("CPU Utilization: %.2f%%\n",
           time > 0 ? total_busy * 100.0 / time : 0.0);
}