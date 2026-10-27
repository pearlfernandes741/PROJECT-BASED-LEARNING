// core.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <mqueue.h>

#define SOCKET_PATH "/tmp/week4_sim.sock"
#define QUEUE_NAME "/week4_sim_queue"
#define FIFO_PATH "core_to_logger.fifo"
#define BUFFER_SIZE 256
#define MAX_ITEMS 20

int main(void)
{
    int server_fd, client_fd, log_fd;
    int stack[MAX_ITEMS], top = -1;
    int queue[MAX_ITEMS], front = 0, rear = -1, count = 0;
    double accumulator = 0;
    unsigned long instructions = 0;

    struct sockaddr_un address;
    struct mq_attr attr = {0, 10, BUFFER_SIZE, 0};
    mqd_t mq;

    char command[BUFFER_SIZE];
    char response[BUFFER_SIZE];
    char operation[20];
    double a, b;
    long value;

    unlink(SOCKET_PATH);
    unlink(FIFO_PATH);
    mq_unlink(QUEUE_NAME);

    if (mkfifo(FIFO_PATH, 0666) == -1) {
        perror("Core: create FIFO");
        return 1;
    }

    mq = mq_open(QUEUE_NAME, O_CREAT | O_WRONLY, 0666, &attr);
    if (mq == (mqd_t)-1) {
        perror("Core: create message queue");
        unlink(FIFO_PATH);
        return 1;
    }

    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Core: socket");
        return 1;
    }

    memset(&address, 0, sizeof(address));
    address.sun_family = AF_UNIX;
    strcpy(address.sun_path, SOCKET_PATH);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == -1) {
        perror("Core: bind");
        return 1;
    }

    if (listen(server_fd, 1) == -1) {
        perror("Core: listen");
        return 1;
    }

    printf("CORE: Socket ready. Waiting for Logger...\n");
    fflush(stdout);

    /* Wait for Logger to open the FIFO's reading end. */
    log_fd = open(FIFO_PATH, O_WRONLY);
    if (log_fd == -1) {
        perror("Core: open logger FIFO");
        return 1;
    }

    dprintf(log_fd, "Core started.\n");
    printf("CORE: Logger connected. Waiting for UI...\n");
    fflush(stdout);

    client_fd = accept(server_fd, NULL, NULL);
    if (client_fd == -1) {
        perror("Core: accept");
        return 1;
    }

    printf("CORE: UI connected. Ready.\n");
    fflush(stdout);

    FILE *client = fdopen(client_fd, "r");
    if (client == NULL) {
        perror("Core: fdopen");
        close(client_fd);
        return 1;
    }

    while (fgets(command, sizeof(command), client) != NULL) {
        command[strcspn(command, "\r\n")] = '\0';
        snprintf(response, sizeof(response), "Unknown command.");

        if (sscanf(command, "%19s", operation) != 1)
            continue;

        if (strcmp(operation, "ADD") == 0 &&
            sscanf(command, "%*s %lf %lf", &a, &b) == 2) {
            accumulator = a + b;
            snprintf(response, sizeof(response),
                     "ADD: %.10g + %.10g = %.10g", a, b, accumulator);

        } else if (strcmp(operation, "SUB") == 0 &&
                   sscanf(command, "%*s %lf %lf", &a, &b) == 2) {
            accumulator = a - b;
            snprintf(response, sizeof(response),
                     "SUBTRACT: %.10g - %.10g = %.10g", a, b, accumulator);

        } else if (strcmp(operation, "MUL") == 0 &&
                   sscanf(command, "%*s %lf %lf", &a, &b) == 2) {
            accumulator = a * b;
            snprintf(response, sizeof(response),
                     "MULTIPLY: %.10g * %.10g = %.10g", a, b, accumulator);

        } else if (strcmp(operation, "DIV") == 0 &&
                   sscanf(command, "%*s %lf %lf", &a, &b) == 2) {
            if (b == 0) {
                snprintf(response, sizeof(response),
                         "ERROR: Cannot divide by zero.");
            } else {
                accumulator = a / b;
                snprintf(response, sizeof(response),
                         "DIVIDE: %.10g / %.10g = %.10g",
                         a, b, accumulator);
            }

        } else if (strcmp(operation, "LOAD") == 0 &&
                   sscanf(command, "%*s %ld", &value) == 1) {
            accumulator = value;
            snprintf(response, sizeof(response),
                     "LOAD: Accumulator = %.10g", accumulator);

        } else if (strcmp(operation, "PUSH") == 0 &&
                   sscanf(command, "%*s %ld", &value) == 1) {
            if (top == MAX_ITEMS - 1) {
                snprintf(response, sizeof(response), "ERROR: Stack is full.");
            } else {
                stack[++top] = (int)value;
                snprintf(response, sizeof(response),
                         "PUSH: %ld added. Stack size = %d",
                         value, top + 1);
            }

        } else if (strcmp(operation, "POP") == 0) {
            if (top < 0) {
                snprintf(response, sizeof(response), "ERROR: Stack is empty.");
            } else {
                value = stack[top--];
                snprintf(response, sizeof(response),
                         "POP: Removed %ld from stack.", value);
            }

        } else if (strcmp(operation, "ENQUEUE") == 0 &&
                   sscanf(command, "%*s %ld", &value) == 1) {
            if (count == MAX_ITEMS) {
                snprintf(response, sizeof(response), "ERROR: Queue is full.");
            } else {
                rear = (rear + 1) % MAX_ITEMS;
                queue[rear] = (int)value;
                count++;
                snprintf(response, sizeof(response),
                         "ENQUEUE: %ld added. Queue size = %d",
                         value, count);
            }

        } else if (strcmp(operation, "DEQUEUE") == 0) {
            if (count == 0) {
                snprintf(response, sizeof(response), "ERROR: Queue is empty.");
            } else {
                value = queue[front];
                front = (front + 1) % MAX_ITEMS;
                count--;
                snprintf(response, sizeof(response),
                         "DEQUEUE: Removed %ld. Queue size = %d",
                         value, count);
            }

        } else if (strcmp(operation, "STATUS") == 0) {
            snprintf(response, sizeof(response),
                     "CORE STATUS\nAccumulator: %.10g\nInstructions: %lu\n"
                     "Stack items: %d\nQueue items: %d",
                     accumulator, instructions, top + 1, count);

        } else if (strcmp(operation, "EXIT") == 0) {
            snprintf(response, sizeof(response), "Core shutting down. Goodbye!");
        }

        instructions++;

        printf("CORE received: %s\n", command);
        printf("CORE response: %s\n", response);
        fflush(stdout);

        dprintf(log_fd, "Command: %s | Response: %s\n", command, response);

        if (mq_send(mq, response, strlen(response) + 1, 0) == -1) {
            perror("Core: send message queue response");
            break;
        }

        if (strcmp(operation, "EXIT") == 0)
            break;
    }

    fclose(client);
    close(log_fd);
    close(server_fd);
    mq_close(mq);
    mq_unlink(QUEUE_NAME);
    unlink(SOCKET_PATH);
    unlink(FIFO_PATH);

    printf("CORE: Process stopped.\n");
    return 0;
}