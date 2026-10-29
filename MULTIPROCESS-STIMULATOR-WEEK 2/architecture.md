# Multi-Process Simulator Architecture

## 1. System Overview

The Multi-Process Simulator consists of three independent processes: UI, Core, and Logger. These processes communicate using different Inter-Process Communication (IPC) mechanisms.

## 2. Architecture Diagram

```text
             USER
               |
               v
       +---------------+
       |   UI Process  |
       |   (Gerald)    |
       +---------------+
               |
               | UNIX Domain Socket
               | /tmp/week4_sim.sock
               v
       +---------------+
       |  Core Process |
       |    (Naman)    |
       +---------------+
          |         |
          |         |
          v         v
   POSIX Message   Named Pipe (FIFO)
       Queue       core_to_logger.fifo
 /week4_sim_queue       |
          |             v
          v       +---------------+
       UI Process | Logger Process|
       receives   |   (Srajana)   |
       responses  +---------------+
                         |
                         v
                  simulator.log
```

## 3. Process Responsibilities

### UI Process — Gerald

- Accepts user input and displays the menu.
- Sends commands to the Core through a UNIX domain socket.
- Receives responses through a POSIX message queue.
- Displays results to the user.

### Core Process — Naman

- Receives commands from the UI.
- Executes simulator instructions.
- Maintains the accumulator, stack, queue, and instruction counter.
- Sends responses to the UI through a POSIX message queue.
- Sends execution details to the Logger through a named pipe.

### Logger Process — Srajana

- Receives log messages from the Core through a named pipe.
- Records simulator events and command results in `simulator.log`.

### Integration and Testing — Pearl

- Integrates the UI, Core, and Logger.
- Checks IPC communication between processes.
- Tests command execution and verifies output.
- Identifies and resolves integration issues.

## 4. Execution Flow

1. The user enters a command through the UI.
2. The UI sends the command to the Core using a UNIX domain socket.
3. The Core processes the command and calculates the result.
4. The Core sends the response to the UI through a POSIX message queue.
5. The Core sends execution details to the Logger through a named pipe.
6. The Logger stores the received details in `simulator.log`.

## 5. Example

For the command:

`ADD 3 5`

The Core calculates the result and returns:

`ADD: 3 + 5 = 8`

The Logger records the command and its response in `simulator.log`.

## 6. Advantages

- Independent execution of UI, Core, and Logger.
- Modular design and separation of responsibilities.
- Different IPC mechanisms for different communication paths.
- Centralized logging for debugging and analysis.
- Easier testing and maintenance of individual components.
