// logger.c
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#define FIFO_PATH "core_to_logger.fifo"
#define LOG_FILE "simulator.log"

int main(void)
{
    int fifo_fd;
    char buffer[512];
    ssize_t bytes_read;
    FILE *log_file;

    printf("LOGGER: Waiting for Core...\n");
    fflush(stdout);

    fifo_fd = open(FIFO_PATH, O_RDONLY);
    if (fifo_fd == -1) {
        perror("Logger: open FIFO");
        return 1;
    }

    log_file = fopen(LOG_FILE, "a");
    if (log_file == NULL) {
        perror("Logger: open log file");
        close(fifo_fd);
        return 1;
    }

    printf("LOGGER: Connected successfully.\n");
    printf("LOGGER: Saving activity to %s\n", LOG_FILE);
    fflush(stdout);

    while ((bytes_read = read(fifo_fd, buffer, sizeof(buffer))) > 0) {
        fwrite(buffer, 1, (size_t)bytes_read, stdout);
        fflush(stdout);

        fwrite(buffer, 1, (size_t)bytes_read, log_file);
        fflush(log_file);
    }

    if (bytes_read == -1)
        perror("Logger: read FIFO");

    printf("LOGGER: Core connection closed.\n");

    fclose(log_file);
    close(fifo_fd);

    return 0;
}