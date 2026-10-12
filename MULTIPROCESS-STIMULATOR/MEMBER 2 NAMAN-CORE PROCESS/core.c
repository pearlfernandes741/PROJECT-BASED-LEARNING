#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>

#define MEMORY_SIZE 100
#define STACK_SIZE 100
#define QUEUE_SIZE 100

#define UI_FIFO "ui_to_core.fifo"
#define CORE_FIFO "core_to_ui.fifo"
#define LOGGER_FIFO "logger_fifo"

int memory[MEMORY_SIZE];

int stack[STACK_SIZE];
int top = -1;

char queue[QUEUE_SIZE][50];
int front = 0;
int rear = -1;

int accumulator = 0;
int programCounter = 0;


/* ================= MEMORY ================= */

void store(int address, int value)
{
    if (address >= 0 && address < MEMORY_SIZE)
    {
        memory[address] = value;
        printf("Memory[%d] = %d\n", address, value);
    }
    else
    {
        printf("Invalid memory address!\n");
    }
}

int load(int address)
{
    if (address >= 0 && address < MEMORY_SIZE)
    {
        return memory[address];
    }
    else
    {
        printf("Invalid memory address!\n");
        return 0;
    }
}


/* ================= STACK ================= */

void push(int value)
{
    if (top == STACK_SIZE - 1)
    {
        printf("Stack Overflow!\n");
        return;
    }

    top++;
    stack[top] = value;

    printf("PUSH %d -> Stack\n", value);
}

int pop()
{
    if (top == -1)
    {
        printf("Stack Underflow!\n");
        return 0;
    }

    int value = stack[top];
    top--;

    printf("POP -> %d\n", value);

    return value;
}


/* ================= QUEUE ================= */

void enqueue(char instruction[])
{
    if (rear == QUEUE_SIZE - 1)
    {
        printf("Queue is full!\n");
        return;
    }

    rear++;

    strcpy(queue[rear], instruction);

    printf("Added to Queue: %s\n", instruction);
}

char *dequeue()
{
    if (front > rear)
    {
        return NULL;
    }

    return queue[front++];
}


/* ================= CPU ================= */

void executeInstruction(char instruction[])
{
    char command[20];
    int value = 0;

    sscanf(instruction, "%19s %d", command, &value);

    printf("\nExecuting: %s\n", instruction);

    if (strcmp(command, "LOAD") == 0)
    {
        accumulator = value;

        printf("ACC = %d\n", accumulator);
    }

    else if (strcmp(command, "ADD") == 0)
    {
        accumulator = accumulator + value;

        printf("ACC = %d\n", accumulator);
    }

    else if (strcmp(command, "SUB") == 0)
    {
        accumulator = accumulator - value;

        printf("ACC = %d\n", accumulator);
    }

    else if (strcmp(command, "MUL") == 0)
    {
        accumulator = accumulator * value;

        printf("ACC = %d\n", accumulator);
    }

    else if (strcmp(command, "DIV") == 0)
    {
        if (value == 0)
        {
            printf("Cannot divide by zero!\n");
            return;
        }

        accumulator = accumulator / value;

        printf("ACC = %d\n", accumulator);
    }

    else if (strcmp(command, "STORE") == 0)
    {
        store(value, accumulator);
    }

    else if (strcmp(command, "LOADM") == 0)
    {
        accumulator = load(value);

        printf("ACC = %d\n", accumulator);
    }

    else if (strcmp(command, "PUSH") == 0)
    {
        push(accumulator);
    }

    else if (strcmp(command, "POP") == 0)
    {
        accumulator = pop();

        printf("ACC = %d\n", accumulator);
    }

    else
    {
        printf("Invalid instruction!\n");
    }
}


/* ================= MAIN / IPC ================= */

int main()
{
    int ui_fd;
    int core_fd;
    int logger_fd;

    char instruction[200];

    printf("=================================\n");
    printf("       CORE PROCESS SIMULATOR\n");
    printf("=================================\n");

    /* Create FIFOs */

    if (mkfifo(UI_FIFO, 0666) == -1 && errno != EEXIST)
    {
        perror("ui_to_core.fifo");
        return 1;
    }

    if (mkfifo(CORE_FIFO, 0666) == -1 && errno != EEXIST)
    {
        perror("core_to_ui.fifo");
        return 1;
    }

    if (mkfifo(LOGGER_FIFO, 0666) == -1 && errno != EEXIST)
    {
        perror("logger_fifo");
        return 1;
    }

    printf("Waiting for UI Process...\n");

    /* UI -> CORE */

    ui_fd = open(UI_FIFO, O_RDONLY);

    if (ui_fd == -1)
    {
        perror("UI FIFO");
        return 1;
    }

    /* CORE -> UI */

    core_fd = open(CORE_FIFO, O_WRONLY);

    if (core_fd == -1)
    {
        perror("CORE FIFO");
        return 1;
    }

    /* CORE -> LOGGER */

    logger_fd = open(LOGGER_FIFO, O_WRONLY);

    if (logger_fd == -1)
    {
        perror("LOGGER FIFO");
        return 1;
    }

    printf("UI connected successfully.\n");
    printf("Logger connected successfully.\n");

    printf("\nWaiting for commands from UI...\n");

    while (1)
    {
        int n = read(ui_fd,
                     instruction,
                     sizeof(instruction) - 1);

        if (n <= 0)
        {
            break;
        }

        instruction[n] = '\0';

        instruction[strcspn(instruction, "\n")] = '\0';

        printf("\nReceived: %s\n", instruction);


        /* EXIT */

        if (strcmp(instruction, "EXIT") == 0)
        {
            char response[] =
                "Core Process Completed.\n";

            write(core_fd,
                  response,
                  strlen(response));

            char log[] =
                "INFO: Core Process Stopped\n";

            write(logger_fd,
                  log,
                  strlen(log));

            break;
        }


        /* STATUS */

        if (strcmp(instruction, "STATUS") == 0)
        {
            char response[200];

            sprintf(response,
                    "Program Counter = %d\n"
                    "Accumulator = %d\n",
                    programCounter,
                    accumulator);

            write(core_fd,
                  response,
                  strlen(response));

            continue;
        }


        /* EXECUTE INSTRUCTION */

        executeInstruction(instruction);

        programCounter++;


        /* Send result to UI */

        char response[300];

        sprintf(response,
                "Program Counter = %d\n"
                "Accumulator = %d\n",
                programCounter,
                accumulator);

        write(core_fd,
              response,
              strlen(response));


        /* Send result to Logger */

        char log[300];

        sprintf(log,
                "INFO: Executed %s | ACC = %d | PC = %d\n",
                instruction,
                accumulator,
                programCounter);

        write(logger_fd,
              log,
              strlen(log));
    }


    close(ui_fd);
    close(core_fd);
    close(logger_fd);

    unlink(UI_FIFO);
    unlink(CORE_FIFO);
    unlink(LOGGER_FIFO);

    printf("\nCore Process Completed Successfully.\n");

    return 0;
}