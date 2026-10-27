// ui.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <mqueue.h>
#include <fcntl.h>

#define SOCKET_PATH "/tmp/week4_sim.sock"
#define QUEUE_NAME "/week4_sim_queue"
#define BUFFER_SIZE 256

int main(void)
{
    int sock, choice;
    struct sockaddr_un address;
    mqd_t mq;
    char command[BUFFER_SIZE];
    char response[BUFFER_SIZE + 1];
    double a, b;
    long value;

    sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("UI: socket");
        return 1;
    }

    memset(&address, 0, sizeof(address));
    address.sun_family = AF_UNIX;
    strcpy(address.sun_path, SOCKET_PATH);

    if (connect(sock, (struct sockaddr *)&address, sizeof(address)) == -1) {
        perror("UI: connect to Core");
        close(sock);
        return 1;
    }

    mq = mq_open(QUEUE_NAME, O_RDONLY);
    if (mq == (mqd_t)-1) {
        perror("UI: open message queue");
        close(sock);
        return 1;
    }

    printf("\n=== MULTI-PROCESS SIMULATOR ===\n");

    while (1) {
        printf("\n1. ADD\n");
        printf("2. SUBTRACT\n");
        printf("3. MULTIPLY\n");
        printf("4. DIVIDE\n");
        printf("5. LOAD VALUE\n");
        printf("6. PUSH TO STACK\n");
        printf("7. POP FROM STACK\n");
        printf("8. ENQUEUE\n");
        printf("9. DEQUEUE\n");
        printf("10. CORE STATUS\n");
        printf("0. EXIT\n");
        printf("Enter choice: ");

        if (scanf("%d", &choice) != 1) {
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF) {}
            printf("Please enter a valid number.\n");
            continue;
        }

        if (choice == 0) {
            snprintf(command, sizeof(command), "EXIT\n");
        } else if (choice >= 1 && choice <= 4) {
            printf("Enter first number: ");
            if (scanf("%lf", &a) != 1) {
                printf("Invalid number.\n");
                int ch;
                while ((ch = getchar()) != '\n' && ch != EOF) {}
                continue;
            }

            printf("Enter second number: ");
            if (scanf("%lf", &b) != 1) {
                printf("Invalid number.\n");
                int ch;
                while ((ch = getchar()) != '\n' && ch != EOF) {}
                continue;
            }

            if (choice == 1)
                snprintf(command, sizeof(command), "ADD %.10g %.10g\n", a, b);
            else if (choice == 2)
                snprintf(command, sizeof(command), "SUB %.10g %.10g\n", a, b);
            else if (choice == 3)
                snprintf(command, sizeof(command), "MUL %.10g %.10g\n", a, b);
            else
                snprintf(command, sizeof(command), "DIV %.10g %.10g\n", a, b);

        } else if (choice == 5 || choice == 6 || choice == 8) {
            printf("Enter value: ");
            if (scanf("%ld", &value) != 1) {
                printf("Invalid value.\n");
                int ch;
                while ((ch = getchar()) != '\n' && ch != EOF) {}
                continue;
            }

            if (choice == 5)
                snprintf(command, sizeof(command), "LOAD %ld\n", value);
            else if (choice == 6)
                snprintf(command, sizeof(command), "PUSH %ld\n", value);
            else
                snprintf(command, sizeof(command), "ENQUEUE %ld\n", value);

        } else if (choice == 7) {
            snprintf(command, sizeof(command), "POP\n");
        } else if (choice == 9) {
            snprintf(command, sizeof(command), "DEQUEUE\n");
        } else if (choice == 10) {
            snprintf(command, sizeof(command), "STATUS\n");
        } else {
            printf("Invalid choice. Try again.\n");
            continue;
        }

        if (send(sock, command, strlen(command), 0) == -1) {
            perror("UI: send");
            break;
        }

        ssize_t n = mq_receive(mq, response, BUFFER_SIZE, NULL);
        if (n == -1) {
            perror("UI: receive response");
            break;
        }

        response[n] = '\0';
        printf("\n%s\n", response);

        if (choice == 0)
            break;
    }

    mq_close(mq);
    close(sock);
    printf("UI process stopped.\n");
    return 0;
}
