// Include the custom header containing resource data,
// process information, constants, and function declarations
#include "simulation.h"

// Define a function to request resources for a process
// process identifies the process requesting resources
// request[] contains the number of units requested for each resource
int request_resources(int process, const int request[])
{
    // Check whether the process number is valid
    if (process < 0 || process >= process_count)

        // Return -1 if the process number is invalid
        return -1;

    // Calculate the remaining resource needs of all processes
    // Return -1 if the calculation fails
    if (!calculate_need())
        return -1;

    // Check the requested quantity of every resource type
    for (int j = 0; j < resource_count; j++) {

        // Reject if the request is negative,
        // exceeds the process's remaining need,
        // or exceeds the currently available resources
        if (request[j] < 0 ||
            request[j] > need[process][j] ||
            request[j] > available[j])

            // Return 0 if the request cannot be granted
            return 0;
    }

    // Store the previous available resources
    int old_available[MAX_RESOURCES];

    // Store the process's previous allocated resources
    int old_allocation[MAX_RESOURCES];

    // Store the process's previous remaining resource needs
    int old_need[MAX_RESOURCES];

    // Temporarily allocate the requested resources
    for (int j = 0; j < resource_count; j++) {

        // Save the current available resource quantity
        old_available[j] = available[j];

        // Save the current allocation for this process
        old_allocation[j] = allocation[process][j];

        // Save the current remaining need for this process
        old_need[j] = need[process][j];

        // Subtract the requested resources from available resources
        available[j] -= request[j];

        // Add the requested resources to the process's allocation
        allocation[process][j] += request[j];

        // Reduce the process's remaining need
        need[process][j] -= request[j];
    }

    // Create an array to store the safe sequence of processes
    int sequence[MAX_PROCESSES];

    // Run Banker's Algorithm to check whether the
    // temporary allocation leaves the system in a safe state
    if (!banker_algorithm(sequence)) {

        // If the system is unsafe, restore the previous values
        for (int j = 0; j < resource_count; j++) {

            // Restore the original available resources
            available[j] = old_available[j];

            // Restore the original allocation
            allocation[process][j] = old_allocation[j];

            // Restore the original remaining need
            need[process][j] = old_need[j];
        }

        // Return 0 because the request was not granted
        return 0;
    }

    // Return 1 because the request was granted safely
    return 1;
}

// Define a function to release resources held by a process
// release[] contains the quantity of each resource to release
int release_resources(int process, const int release[])
{
    // Check whether the process number is valid
    if (process < 0 || process >= process_count)

        // Return -1 if the process number is invalid
        return -1;

    // Validate the release quantity for every resource type
    for (int j = 0; j < resource_count; j++) {

        // Check whether the release is negative or
        // greater than the resources allocated to the process
        if (release[j] < 0 ||
            release[j] > allocation[process][j])

            // Return 0 if the release request is invalid
            return 0;
    }

    // Release the specified resources
    for (int j = 0; j < resource_count; j++) {

        // Subtract released resources from the process's allocation
        allocation[process][j] -= release[j];

        // Add released resources back to the available resources
        available[j] += release[j];
    }

    // Recalculate the remaining needs after releasing resources
    // Return the result of calculate_need()
    return calculate_need();
}

// Define a function to recover a process by releasing
// all resources currently allocated to that process
int recover_process(int process)
{
    // Check whether the process number is valid
    if (process < 0 || process >= process_count)

        // Return -1 if the process number is invalid
        return -1;

    // Process every resource type
    for (int j = 0; j < resource_count; j++) {

        // Return all allocated resources to the available pool
        available[j] += allocation[process][j];

        // Reset the process's allocated resources to zero
        allocation[process][j] = 0;

        // Reset the process's maximum resource demand to zero
        maximum[process][j] = 0;

        // Reset the process's remaining resource need to zero
        need[process][j] = 0;
    }

    // Return 1 to indicate successful recovery
    return 1;
}