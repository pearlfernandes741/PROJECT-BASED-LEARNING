#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>

#define UI_FIFO "ui_to_core.fifo"
#define CORE_FIFO "core_to_ui.fifo"
#define LOGGER_FIFO "core_to_logger.fifo"

#define MEMORY_SIZE 100
#define STACK_SIZE 100

int memory[MEMORY_SIZE];

int stack[STACK_SIZE];
int top = -1;

int accumulator = 0;
int programCounter = 0;


/* ---------- SEND TO LOGGER ---------- */

void logMessage(int logger_fd, char *message)
{
    if (logger_fd != -1)
    {
        write(logger_fd, message, strlen(message));
    }
}


/* ---------- MEMORY ---------- */

void store(int address, int value)
{
    if (address >= 0 && address < MEMORY_SIZE)
    {
        memory[address] = value;
    }
}

int load(int address)
{
    if (address >= 0 && address < MEMORY_SIZE)
    {
        return memory[address];
    }

    return 0;
}


/* ---------- STACK ---------- */

void push(int value)
{
    if (top < STACK_SIZE - 1)
    {
        top++;
        stack[top] = value;
    }
}

int pop()
{
    if (top == -1)
    {
        return 0;
    }

    int value = stack[top];
    top--;

    return value;
}


/* ---------- EXECUTE INSTRUCTION ---------- */

void executeInstruction(char *instruction, char *response)
{
    char command[20];
    int value;

    value = 0;

    sscanf(instruction, "%19s %d", command, &value);

    if (strcmp(command, "LOAD") == 0)
    {
        accumulator = value;

        sprintf(response,
                "Executed LOAD %d | ACC = %d\n",
                value, accumulator);
    }

    else if (strcmp(command, "ADD") == 0)
    {
        accumulator = accumulator + value;

        sprintf(response,
                "Executed ADD %d | ACC = %d\n",
                value, accumulator);
    }

    else if (strcmp(command, "SUB") == 0)
    {
        accumulator = accumulator - value;

        sprintf(response,
                "Executed SUB %d | ACC = %d\n",
                value, accumulator);
    }

    else if (strcmp(command, "STORE") == 0)
    {
        store(value, accumulator);

        sprintf(response,
                "Executed STORE %d | Memory[%d] = %d\n",
                value, value, accumulator);
    }

    else if (strcmp(command, "LOADM") == 0)
    {
        accumulator = load(value);

        sprintf(response,
                "Executed LOADM %d | ACC = %d\n",
                value, accumulator);
    }

    else if (strcmp(command, "PUSH") == 0)
    {
        push(accumulator);

        sprintf(response,
                "Executed PUSH | Value = %d\n",
                accumulator);
    }

    else if (strcmp(command, "POP") == 0)
    {
        accumulator = pop();

        sprintf(response,
                "Executed POP | ACC = %d\n",
                accumulator);
    }

    else
    {
        sprintf(response,
                "Invalid instruction\n");
    }

    programCounter++;
}


/* ---------- MAIN ---------- */

int main()
{
    int ui_in;
    int ui_out;
    int logger_out;

    char instruction[200];
    char response[200];

    printf("=================================\n");
    printf("       CORE PROCESS\n");
    printf("=================================\n");

    /* Create Logger FIFO */

    if (mkfifo(LOGGER_FIFO, 0666) == -1 && errno != EEXIST)
    {
        perror("Logger FIFO");
        return 1;
    }

    printf("Waiting for UI...\n");

    ui_in = open(UI_FIFO, O_RDONLY);

    if (ui_in == -1)
    {
        perror("UI FIFO");
        return 1;
    }

    ui_out = open(CORE_FIFO, O_WRONLY);

    if (ui_out == -1)
    {
        perror("Core FIFO");
        close(ui_in);
        return 1;
    }

    printf("UI connected.\n");

    /* Connect to Logger */

    logger_out = open(LOGGER_FIFO, O_WRONLY);

    if (logger_out == -1)
    {
        perror("Logger connection");
    }
    else
    {
        printf("Logger connected.\n");
    }


    /* Receive commands from UI */

    while (1)
    {
        int n = read(ui_in, instruction, sizeof(instruction) - 1);

        if (n <= 0)
        {
            break;
        }

        instruction[n] = '\0';

        /* Remove newline */

        instruction[strcspn(instruction, "\n")] = '\0';


        /* EXIT */

        if (strcmp(instruction, "EXIT") == 0)
        {
            char message[] = "Core process shutting down\n";

            logMessage(logger_out, message);

            write(ui_out,
                  "Core process exiting\n",
                  21);

            break;
        }


        /* STATUS */

        if (strcmp(instruction, "STATUS") == 0)
        {
            sprintf(response,
                    "PC = %d | ACC = %d | Stack Top = %d\n",
                    programCounter,
                    accumulator,
                    top);

            write(ui_out, response, strlen(response));

            logMessage(logger_out, response);

            continue;
        }


        /* NORMAL INSTRUCTION */

        executeInstruction(instruction, response);

        printf("%s", response);

        /* Send result to UI */

        write(ui_out,
              response,
              strlen(response));

        /* Send result to Logger */

        logMessage(logger_out, response);
    }


    close(ui_in);
    close(ui_out);

    if (logger_out != -1)
        close(logger_out);

    printf("Core Process closed.\n");

    return 0;
}