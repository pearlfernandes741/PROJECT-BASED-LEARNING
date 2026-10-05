#include <stdio.h>
#include <string.h>

#define MEMORY_SIZE 100
#define STACK_SIZE 100
#define QUEUE_SIZE 100

// ---------------- MEMORY ----------------
int memory[MEMORY_SIZE];

// ---------------- STACK ----------------
int stack[STACK_SIZE];
int top = -1;

// ---------------- QUEUE ----------------
char queue[QUEUE_SIZE][50];
int front = 0;
int rear = -1;

// ---------------- CPU ----------------
int accumulator = 0;
int programCounter = 0;


// ---------- MEMORY FUNCTIONS ----------

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


// ---------- STACK FUNCTIONS ----------

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


// ---------- QUEUE FUNCTIONS ----------

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

char* dequeue()
{
    if (front > rear)
    {
        return NULL;
    }

    return queue[front++];
}


// ---------- CPU EXECUTION ----------

void executeInstruction(char instruction[])
{
    char command[20];
    int value;

    sscanf(instruction, "%s %d", command, &value);

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


// ---------- MAIN ----------

int main()
{
    printf("=================================\n");
    printf("       CORE PROCESS SIMULATOR\n");
    printf("=================================\n");

    // Add instructions to Queue

    enqueue("LOAD 10");
    enqueue("ADD 5");
    enqueue("PUSH");
    enqueue("LOAD 20");
    enqueue("POP");
    enqueue("STORE 50");
    enqueue("LOADM 50");

    printf("\n=================================\n");
    printf("       CPU EXECUTION\n");
    printf("=================================\n");

    // Execute instructions from Queue

    while (front <= rear)
    {
        char *instruction = dequeue();

        executeInstruction(instruction);

        programCounter++;
    }

    printf("\n=================================\n");
    printf("       FINAL CPU STATE\n");
    printf("=================================\n");

    printf("Program Counter = %d\n", programCounter);
    printf("Accumulator = %d\n", accumulator);

    printf("\nCore Process Completed Successfully.\n");

    return 0;
}