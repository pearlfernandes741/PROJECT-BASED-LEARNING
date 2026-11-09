// logger.c

// Include standard input/output functions like printf, fopen, and fwrite
#include <stdio.h>

// Include POSIX functions like read and close
#include <unistd.h>

// Include file control functions like open and the O_RDONLY flag
#include <fcntl.h>

// Define the name of the FIFO used for communication
#define FIFO_PATH "core_to_logger.fifo"

// Define the file where log messages will be saved
#define LOG_FILE "simulator.log"

// Main function where program execution begins
int main(void)
{
    // Store the file descriptor of the FIFO
    int fifo_fd;

    // Create a buffer to temporarily store up to 512 bytes
    char buffer[512];

    // Store the number of bytes received from the FIFO
    ssize_t bytes_read;

    // Declare a file pointer for the log file
    FILE *log_file;

    // Display a message indicating the logger is waiting
    printf("LOGGER: Waiting for Core...\n");

    // Immediately display any buffered output
    fflush(stdout);

    // Open the FIFO in read-only mode
    // This may wait until another process opens the FIFO for writing
    fifo_fd = open(FIFO_PATH, O_RDONLY);

    // Check whether opening the FIFO failed
    if (fifo_fd == -1) {

        // Display the error message
        perror("Logger: open FIFO");

        // Exit the program with an error status
        return 1;
    }

    // Open the log file in append mode
    // Existing content is preserved and new data is added at the end
    log_file = fopen(LOG_FILE, "a");

    // Check whether opening the log file failed
    if (log_file == NULL) {

        // Display the error message
        perror("Logger: open log file");

        // Close the FIFO before exiting
        close(fifo_fd);

        // Exit the program with an error status
        return 1;
    }

    // Display a successful connection message
    printf("LOGGER: Connected successfully.\n");

    // Display the name of the log file
    printf("LOGGER: Saving activity to %s\n", LOG_FILE);

    // Immediately display the messages on the terminal
    fflush(stdout);

    // Continuously read data from the FIFO
    // Continue while the number of bytes read is greater than zero
    while ((bytes_read = read(fifo_fd, buffer, sizeof(buffer))) > 0) {

        // Write the received bytes to the terminal
        fwrite(buffer, 1, (size_t)bytes_read, stdout);

        // Immediately display the terminal output
        fflush(stdout);

        // Write the same received bytes into the log file
        fwrite(buffer, 1, (size_t)bytes_read, log_file);

        // Immediately flush buffered log data to the file
        fflush(log_file);
    }

    // Check whether reading from the FIFO failed
    if (bytes_read == -1)

        // Display the FIFO reading error
        perror("Logger: read FIFO");

    // Display a message indicating the connection has closed
    printf("LOGGER: Core connection closed.\n");

    // Close the log file
    fclose(log_file);

    // Close the FIFO file descriptor
    close(fifo_fd);

    // End the program successfully
    return 0;
}