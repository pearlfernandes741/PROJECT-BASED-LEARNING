#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>

#define UI_FIFO "ui_to_core.fifo"
#define CORE_FIFO "core_to_ui.fifo"
#define LOGGER_FIFO "logger_fifo"

int accumulator = 0;

int main()
{
    int ui_fd, response_fd, logger_fd;
    char instruction[200];
    char command[20];
    int value;

    mkfifo(UI_FIFO, 0666);
    mkfifo(CORE_FIFO, 0666);
    mkfifo(LOGGER_FIFO, 0666);

    printf("Naman Core Process Started...\n");

    ui_fd = open(UI_FIFO, O_RDONLY);
    response_fd = open(CORE_FIFO, O_WRONLY);
    logger_fd = open(LOGGER_FIFO, O_WRONLY);

    printf("Core connected to UI and Logger.\n");

    while (1)
    {
        int n = read(ui_fd, instruction, sizeof(instruction) - 1);

        if (n <= 0)
            continue;

        instruction[n] = '\0';

        instruction[strcspn(instruction, "\n")] = '\0';

        if (strcmp(instruction, "EXIT") == 0)
        {
            write(response_fd, "Core shutting down.\n", 20);
            break;
        }

        if (strcmp(instruction, "STATUS") == 0)
        {
            char status[100];
            sprintf(status, "Accumulator = %d\n", accumulator);
            write(response_fd, status, strlen(status));
            continue;
        }

        strcpy(command, "");
        value = 0;

        sscanf(instruction, "%19s %d", command, &value);

        if (strcmp(command, "LOAD") == 0)
        {
            accumulator = value;
        }
        else if (strcmp(command, "ADD") == 0)
        {
            accumulator += value;
        }
        else if (strcmp(command, "SUB") == 0)
        {
            accumulator -= value;
        }
        else
        {
            write(response_fd, "Invalid instruction\n", 20);
            continue;
        }

        char response[100];
        sprintf(response, "CPU: %d\n", accumulator);

        write(response_fd, response, strlen(response));

        char log[150];
        sprintf(log, "Received: %s\n", instruction);
        write(logger_fd, log, strlen(log));
    }

    close(ui_fd);
    close(response_fd);
    close(logger_fd);

    return 0;
}
