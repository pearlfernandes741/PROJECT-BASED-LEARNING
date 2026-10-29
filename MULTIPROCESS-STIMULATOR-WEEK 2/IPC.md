# Inter-Process Communication (IPC)

## 1. IPC Mechanisms Used

The Multi-Process Simulator uses three IPC mechanisms to exchange commands, responses, and log messages between independent processes.

## 2. UNIX Domain Socket — UI to Core

**Socket path:** `/tmp/week4_sim.sock`

The UI communicates with the Core through a UNIX domain socket.

The UI connects to the socket and sends commands. The Core accepts the connection and receives the commands for execution.

Functions used include:

- `socket()` — creates the socket.
- `bind()` — associates the socket with its path.
- `listen()` — waits for incoming connections.
- `accept()` — accepts the UI connection.
- `connect()` — connects the UI to the Core.

**Why it is used:** It provides local, bidirectional communication between the UI and Core processes.

## 3. POSIX Message Queue — Core to UI

**Queue name:** `/week4_sim_queue`

The Core sends command results to the UI through a POSIX message queue. The UI receives these responses and displays them to the user.

Functions used include:

- `mq_open()` — opens or creates the message queue, depending on the flags used.
- `mq_send()` — sends a response.
- `mq_receive()` — receives a response.

**Why it is used:** It provides a separate communication channel for returning results from the Core to the UI.

## 4. Named Pipe (FIFO) — Core to Logger

**FIFO file:** `core_to_logger.fifo`

The Core writes execution details to the FIFO. The Logger reads these messages and records them in `simulator.log`.

Functions used include:

- `mkfifo()` — creates the named pipe.
- `open()` — opens the FIFO.
- `read()` — reads log messages.
- `dprintf()` — writes formatted log messages to the FIFO file descriptor in the Core implementation.

**Why it is used:** It allows the Core and Logger to exchange log information through a named pipe.

## 5. Communication Flow

1. UI sends a command to Core using a UNIX domain socket.
2. Core executes the command.
3. Core returns the result using a POSIX message queue.
4. Core sends execution details through a named pipe.
5. Logger records the received information in `simulator.log`.

## 6. Advantages

- Processes communicate without combining their code into one executable.
- Each process has a separate responsibility.
- Different communication mechanisms support different data flows.
- Logging is separated from command execution.

## 7. Limitations

- Processes must be started in a compatible order.
- IPC setup and cleanup require additional code.
- Communication failures can affect the overall execution.
- Multiple IPC mechanisms increase implementation complexity.
