#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
    int fd;
    char buffer[256];
    int bytes_read;

    printf("Logger process started...\n");
    printf("Waiting for messages...\n");

    fd = open("logger_fifo", O_RDONLY);

    if (fd == -1)
    {
        perror("Error opening FIFO");
        return 1;
    }

    FILE *log_file = fopen("simulator.log", "a");

    if (log_file == NULL)
    {
        perror("Error opening log file");
        close(fd);
        return 1;
    }
    
    while (1)
{
    bytes_read = read(fd, buffer, sizeof(buffer) - 1);

    if (bytes_read > 0)
    {
        buffer[bytes_read] = '\0';

        printf("Received: %s", buffer);
        fprintf(log_file, "%s", buffer);
        fflush(log_file);
    }
}

    fclose(log_file);
    close(fd);

    return 0;
}