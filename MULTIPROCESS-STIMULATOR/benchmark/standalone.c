#include <stdio.h>
#include <time.h>

int main()
{
    int accumulator = 0;
    int memory[100] = {0};
    int stack[100];
    int top = -1;
    int programCounter = 0;

    struct timespec start, end;

    clock_gettime(CLOCK_MONOTONIC, &start);

    printf("========================================\n");
    printf("       STANDALONE SIMULATOR\n");
    printf("========================================\n");

    /* 1. LOAD 10 */
    accumulator = 10;
    programCounter++;

    printf("Executed LOAD 10 | ACC = %d\n",
           accumulator);

    /* 2. ADD 5 */
    accumulator = accumulator + 5;
    programCounter++;

    printf("Executed ADD 5 | ACC = %d\n",
           accumulator);

    /* 3. PUSH */
    stack[++top] = accumulator;
    programCounter++;

    printf("Executed PUSH | Value = %d\n",
           stack[top]);

    /* 4. LOAD 20 */
    accumulator = 20;
    programCounter++;

    printf("Executed LOAD 20 | ACC = %d\n",
           accumulator);

    /* 5. POP */
    accumulator = stack[top--];
    programCounter++;

    printf("Executed POP | ACC = %d\n",
           accumulator);

    /* 6. STORE 50 */
    memory[50] = accumulator;
    programCounter++;

    printf("Executed STORE 50 | Memory[50] = %d\n",
           memory[50]);

    /* 7. LOADM 50 */
    accumulator = memory[50];
    programCounter++;

    printf("Executed LOADM 50 | ACC = %d\n",
           accumulator);

    clock_gettime(CLOCK_MONOTONIC, &end);

    double execution_time =
        (end.tv_sec - start.tv_sec) +
        (end.tv_nsec - start.tv_nsec) / 1000000000.0;

    printf("\n========================================\n");
    printf("       STANDALONE RESULTS\n");
    printf("========================================\n");

    printf("Execution Time: %.6f seconds\n",
           execution_time);

    printf("Instructions Executed: %d\n",
           programCounter);

    printf("Final Accumulator: %d\n",
           accumulator);

    printf("Final Stack Top: %d\n",
           top);

    printf("Memory[50]: %d\n",
           memory[50]);

    return 0;
}
