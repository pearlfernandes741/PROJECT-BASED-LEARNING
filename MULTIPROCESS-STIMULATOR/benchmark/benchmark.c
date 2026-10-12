#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <signal.h>

double get_time()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

int main()
{
    pid_t logger_pid;
    pid_t core_pid;
    pid_t ui_pid;

    double start, end;

    printf("========================================\n");
    printf("       MULTI-PROCESS BENCHMARK\n");
    printf("========================================\n");

    start = get_time();

    /* Start Logger */
    logger_pid = fork();

    if (logger_pid == 0)
    {
        if (chdir("..") != 0)
        {
            perror("chdir");
            exit(1);
        }

        execl("./logger_program", "./logger_program", NULL);

        perror("Failed to start logger");
        exit(1);
    }

    sleep(1);

    /* Start Core */
    core_pid = fork();

    if (core_pid == 0)
    {
        if (chdir("..") != 0)
        {
            perror("chdir");
            exit(1);
        }

        execl("./core_program", "./core_program", NULL);

        perror("Failed to start core");
        exit(1);
    }

    sleep(1);

    /* Start UI */
    ui_pid = fork();

    if (ui_pid == 0)
    {
        if (freopen("benchmark_input.txt", "r", stdin) == NULL)
        {
            perror("benchmark_input.txt");
            exit(1);
        }

        if (chdir("..") != 0)
        {
            perror("chdir");
            exit(1);
        }

        execl("./ui_program", "./ui_program", NULL);

        perror("Failed to start UI");
        exit(1);
    }

    /* Wait for UI */
    waitpid(ui_pid, NULL, 0);

    /* Wait for Core */
    waitpid(core_pid, NULL, 0);

    /* Stop Logger */
    kill(logger_pid, SIGTERM);
    waitpid(logger_pid, NULL, 0);

    end = get_time();

    printf("\n========================================\n");
    printf("         BENCHMARK RESULTS\n");
    printf("========================================\n");

    printf("Execution Time: %.2f seconds\n",
           end - start);

    printf("========================================\n");

    return 0;
}