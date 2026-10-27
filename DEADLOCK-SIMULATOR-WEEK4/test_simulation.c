#include <stdio.h>
#include "simulation.h"

int main(void)
{
    int request[MAX_RESOURCES] = {0};
    int release[MAX_RESOURCES] = {0};

    printf("=== MEMBER 3 SIMULATION TEST ===\n");

    /* Test 1: Invalid process */
    int result = request_resources(-1, request);
    printf("Test 1 - Invalid request: %s\n",
           result == -1 ? "PASS" : "FAIL");

    /* Test 2: Invalid release */
    result = release_resources(-1, release);
    printf("Test 2 - Invalid release: %s\n",
           result == -1 ? "PASS" : "FAIL");

    /* Test 3: Invalid recovery */
    result = recover_process(-1);
    printf("Test 3 - Invalid recovery: %s\n",
           result == -1 ? "PASS" : "FAIL");

    return 0;
}