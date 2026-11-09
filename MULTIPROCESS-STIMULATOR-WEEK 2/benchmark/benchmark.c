#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

#define UI_TIMEOUT 30
#define OPERATIONS 1000

static double now_seconds(void)
{
    struct timespec t;
    clock_gettime(CLOCK_MONOTONIC, &t);
    return t.tv_sec + t.tv_nsec / 1e9;
}

static pid_t start_process(const char *program, int is_ui)
{
    pid_t pid = fork();

    if (pid != 0)
        return pid;

    if (chdir("..") == -1)
        _exit(1);

    if (is_ui) {
        int input = open("benchmark/benchmark_input.txt", O_RDONLY);
        int output = open("/dev/null", O_WRONLY);

        if (input == -1 || output == -1)
            _exit(1);

        dup2(input, STDIN_FILENO);
        dup2(output, STDOUT_FILENO);
        close(input);
        close(output);
    }

    execl(program, program, (char *)NULL);
    perror("exec");
    _exit(127);
}

static int wait_for_path(const char *path, int seconds)
{
    for (int i = 0; i < seconds * 20; i++) {
        if (access(path, F_OK) == 0)
            return 1;
        usleep(50000);
    }
    return 0;
}

static int wait_for_ui(pid_t pid, int seconds, int *status)
{
    for (int i = 0; i < seconds * 20; i++) {
        pid_t result = waitpid(pid, status, WNOHANG);

        if (result == pid)
            return 1;

        if (result == -1 && errno != EINTR)
            return 0;

        usleep(50000);
    }
    return 0;
}

static void stop_process(pid_t pid)
{
    if (pid <= 0)
        return;

    if (kill(pid, SIGTERM) == -1 && errno == ESRCH)
        return;

    for (int i = 0; i < 20; i++) {
        if (waitpid(pid, NULL, WNOHANG) == pid)
            return;
        usleep(50000);
    }

    kill(pid, SIGKILL);
    waitpid(pid, NULL, 0);
}

int main(void)
{
    pid_t core_pid = -1, logger_pid = -1, ui_pid = -1;
    int status = 0;
    double start, elapsed;
    int ui_reaped = 0;

    printf("========================================\n");
    printf(" UPDATED MULTI-PROCESS BENCHMARK\n");
    printf("========================================\n");
    printf("IPC: UNIX socket + POSIX message queue + FIFO\n");
    printf("Operations: %d additions\n\n", OPERATIONS);
    fflush(stdout);

    start = now_seconds();

    /* Core creates the FIFO and prepares the socket and message queue. */
    core_pid = start_process("./core_program", 0);
    if (core_pid < 0) {
        perror("Start Core");
        goto failure;
    }

    if (!wait_for_path("../core_to_logger.fifo", 5) ||
        !wait_for_path("/tmp/week4_sim.sock", 5)) {
        fprintf(stderr, "ERROR: Core did not create its IPC endpoints.\n");
        goto failure;
    }

    /* Logger opens the FIFO created by Core. */
    logger_pid = start_process("./logger_program", 0);
    if (logger_pid < 0) {
        perror("Start Logger");
        goto failure;
    }

    /* Allow the FIFO connection to complete before launching the UI. */
    usleep(300000);

    ui_pid = start_process("./ui_program", 1);
    if (ui_pid < 0) {
        perror("Start UI");
        goto failure;
    }

    if (!wait_for_ui(ui_pid, UI_TIMEOUT, &status)) {
        fprintf(stderr, "ERROR: UI timed out after %d seconds.\n",
                UI_TIMEOUT);
        goto failure;
    }
    ui_reaped = 1;
    ui_pid = -1;

    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
        fprintf(stderr, "ERROR: UI did not exit successfully.\n");
        goto failure;
    }

    elapsed = now_seconds() - start;

    stop_process(core_pid);
    core_pid = -1;
    stop_process(logger_pid);
    logger_pid = -1;

    printf("\n========================================\n");
    printf(" BENCHMARK RESULTS\n");
    printf("========================================\n");
    printf("Status: SUCCESS\n");
    printf("IPC methods: Socket, POSIX message queue, FIFO\n");
    printf("Completed operations: %d additions\n", OPERATIONS);
    printf("Total elapsed time: %.6f seconds\n", elapsed);
    printf("Total elapsed time: %.3f milliseconds\n", elapsed * 1000.0);
    printf("========================================\n");
    return 0;

failure:
    if (ui_pid > 0)
        stop_process(ui_pid);
    if (core_pid > 0)
        stop_process(core_pid);
    if (logger_pid > 0)
        stop_process(logger_pid);

    if (!ui_reaped) {
        printf("\nStatus: FAILED - no valid timing recorded\n");
    }
    printf("Check the IPC startup and process errors above.\n");
    return 1;
}

