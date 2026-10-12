# Logging Process

## Description
This module implements the logging process for the project. It receives log messages from other processes using IPC and stores them in a log file.

## IPC Method
A POSIX Named Pipe (FIFO) named `logger_fifo` is used for interprocess communication.

## Files
- `logger.c` - Contains the logging process implementation.
- `simulator.log` - Stores the received log messages.

## Working
1. The logger waits for messages through `logger_fifo`.
2. It receives messages from other processes.
3. The received messages are displayed on the terminal.
4. The messages are appended to `simulator.log`.

## Compilation
```bash
gcc logger.c -o logger
```

## Execution
```bash
./logger
```