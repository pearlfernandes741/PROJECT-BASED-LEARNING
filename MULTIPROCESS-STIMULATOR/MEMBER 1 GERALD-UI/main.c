#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <ctype.h>

#define UI_FIFO   "ui_to_core.fifo"
#define CORE_FIFO "core_to_ui.fifo"
#define SIZE 200

void createFIFO(char *name)
{
    if (mkfifo(name, 0666) == -1 && errno != EEXIST)
    {
        perror("FIFO");
        exit(1);
    }
}

void sendCommand(int fd, char *cmd)
{
    write(fd, cmd, strlen(cmd));
}

void showMenu()
{
    printf("\n====================================\n");
    printf("       MULTI-PROCESS SIMULATOR\n");
    printf("              UI PROCESS\n");
    printf("====================================\n");
    printf("1. Execute Instruction\n");
    printf("2. Run Demo Program\n");
    printf("3. Core Status\n");
    printf("4. Exit\n");
    printf("Enter choice: ");
}

void demo(int out, int in)
{
    char *program[] = {
        "LOAD 10\n",
        "ADD 5\n",
        "PUSH\n",
        "LOAD 20\n",
        "POP\n",
        "STORE 50\n",
        "LOADM 50\n"
    };

    char response[SIZE];

    printf("\n--- DEMO PROGRAM ---\n");

    for (int i = 0; i < 7; i++)
    {
        printf(">> %s", program[i]);

        sendCommand(out, program[i]);

        int n = read(in, response, SIZE - 1);
        response[n] = '\0';

        printf("%s\n", response);
    }
}

int validInstruction(char *cmd)
{
    char word[20];
    int value;

    if (sscanf(cmd, "%19s %d", word, &value) == 2)
    {
        if (!strcmp(word, "LOAD")  ||
            !strcmp(word, "ADD")   ||
            !strcmp(word, "SUB")   ||
            !strcmp(word, "STORE") ||
            !strcmp(word, "LOADM"))
            return 1;
    }

    if (!strcmp(cmd, "PUSH") || !strcmp(cmd, "POP"))
        return 1;

    return 0;
}

int main()
{
    int out, in, choice;
    char cmd[SIZE], response[SIZE];

    createFIFO(UI_FIFO);
    createFIFO(CORE_FIFO);

    printf("Waiting for Core Process...\n");

    out = open(UI_FIFO, O_WRONLY);
    in  = open(CORE_FIFO, O_RDONLY);

    if (out == -1 || in == -1)
    {
        perror("Core connection");
        return 1;
    }

    printf("UI connected to Core successfully.\n");

    while (1)
    {
        showMenu();

        if (scanf("%d", &choice) != 1)
        {
            while (getchar() != '\n');
            printf("Invalid choice!\n");
            continue;
        }

        getchar();

        /* Execute one instruction */
        if (choice == 1)
        {
            printf("\nEnter instruction: ");
            fgets(cmd, SIZE, stdin);

            cmd[strcspn(cmd, "\n")] = '\0';

            for (int i = 0; cmd[i]; i++)
                cmd[i] = toupper(cmd[i]);

            if (!validInstruction(cmd))
            {
                printf("Invalid instruction!\n");
                continue;
            }

            strcat(cmd, "\n");
            sendCommand(out, cmd);

            int n = read(in, response, SIZE - 1);
            response[n] = '\0';

            printf("Core: %s\n", response);
        }

        /* Demo */
        else if (choice == 2)
        {
            demo(out, in);
        }

        /* Status */
        else if (choice == 3)
        {
            sendCommand(out, "STATUS\n");

            int n = read(in, response, SIZE - 1);
            response[n] = '\0';

            printf("\n--- CORE STATUS ---\n");
            printf("%s\n", response);
        }

        /* Exit */
        else if (choice == 4)
        {
            sendCommand(out, "EXIT\n");

            close(out);
            close(in);

            unlink(UI_FIFO);
            unlink(CORE_FIFO);

            printf("UI Process closed.\n");
            break;
        }

        else
        {
            printf("Invalid choice!\n");
        }
    }

    return 0;
}
