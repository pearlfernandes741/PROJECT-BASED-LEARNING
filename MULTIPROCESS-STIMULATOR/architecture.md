# Multiprocess Simulator – Architecture

## Overall Architecture

The Multiprocess Simulator is designed using three independent processes:

* **UI Process** – interacts with the user and sends commands.
* **Core Process** – executes instructions and maintains the simulator state.
* **Logger Process** – receives execution information and records it.

The processes communicate using **Named Pipes (FIFOs)**.

```text
                         MULTIPROCESS SIMULATOR
                                  │
              ┌───────────────────┼───────────────────┐
              │                   │                   │
              ▼                   ▼                   ▼
        ┌──────────┐        ┌──────────┐        ┌──────────┐
        │    UI    │        │   CORE   │        │  LOGGER  │
        │  Process │        │  Process │        │  Process │
        └────┬─────┘        └────┬─────┘        └────┬─────┘
             │                   │                   │
             │                   │                   │
             │ ui_to_core.fifo   │                   │
             ├──────────────────►│                   │
             │                   │                   │
             │ core_to_ui.fifo   │                   │
             │◄──────────────────┤                   │
             │                   │                   │
             │                   │ logger_fifo        │
             │                   ├──────────────────►│
             │                   │                   │
             │                   │                   ▼
             │                   │            simulator.log
```

## Components

### UI Process

The UI Process is the user-facing part of the simulator.

It:

* Displays the simulator menu.
* Accepts instructions from the user.
* Validates the entered instruction.
* Sends commands to the Core process.
* Receives responses from the Core.
* Displays execution results.
* Provides options for individual instruction execution, demo execution, Core status, and exit.

The UI communicates with Core using two FIFOs:

* `ui_to_core.fifo`
* `core_to_ui.fifo`

### Core Process

The Core Process is the main processing unit of the simulator.

It:

* Receives instructions from the UI.
* Executes the instructions.
* Maintains the accumulator.
* Maintains the program counter.
* Manages memory.
* Manages the stack.
* Processes instructions such as `LOAD`, `ADD`, `SUB`, `MUL`, `DIV`, `STORE`, `LOADM`, `PUSH`, and `POP`.
* Sends execution results back to the UI.
* Sends execution information to the Logger.

The Core therefore acts as the central component connecting the UI and Logger.

### Logger Process

The Logger Process is responsible for receiving messages from the Core.

It:

* Opens `logger_fifo` for reading.
* Waits for messages from the Core.
* Displays received messages.
* Writes received messages into `simulator.log`.
* Flushes the log file so that the information is written immediately.

## IPC Communication

Three Named Pipes are used in the system:

| FIFO              | Direction     | Purpose                           |
| ----------------- | ------------- | --------------------------------- |
| `ui_to_core.fifo` | UI → Core     | Sends commands/instructions       |
| `core_to_ui.fifo` | Core → UI     | Sends execution results/responses |
| `logger_fifo`     | Core → Logger | Sends execution/log information   |

### UI to Core

The user enters an instruction through the UI.

```text
User
  ↓
UI Process
  ↓
ui_to_core.fifo
  ↓
Core Process
```

The Core reads the instruction from the FIFO and executes it.

### Core to UI

After processing the instruction, the Core sends the result back.

```text
Core Process
     ↓
core_to_ui.fifo
     ↓
UI Process
     ↓
User
```

This allows the UI to display the result of the operation.

### Core to Logger

The Core also sends execution information to the Logger.

```text
Core Process
     ↓
logger_fifo
     ↓
Logger Process
     ↓
simulator.log
```

This allows execution activity to be recorded separately from the main UI.

## Internal Core Processing

The Core maintains the internal state required for instruction execution.

```text
                    CORE PROCESS
                         │
        ┌────────────────┼────────────────┐
        │                │                │
        ▼                ▼                ▼
   Accumulator       Program Counter    Memory
        │
        │
        └──────────────┬─────────────────┐
                       │                 │
                       ▼                 ▼
                     Stack          Instruction
                                      Execution
```

For example, during the demo:

```text
LOAD 10
   ↓
ACC = 10

ADD 5
   ↓
ACC = 15

PUSH
   ↓
15 stored on Stack

LOAD 20
   ↓
ACC = 20

POP
   ↓
ACC = 15

STORE 50
   ↓
Memory[50] = 15

LOADM 50
   ↓
ACC = 15
```

## Complete Execution Flow

The complete communication and processing flow is:

```text
                USER
                  │
                  ▼
             UI PROCESS
                  │
                  │ Command
                  ▼
          ui_to_core.fifo
                  │
                  ▼
            CORE PROCESS
                  │
          ┌───────┴────────┐
          │                │
          │ Execute        │ Send log
          │ instruction    │
          ▼                ▼
     Core State       logger_fifo
          │                │
          │                ▼
          │           LOGGER PROCESS
          │                │
          │                ▼
          │          simulator.log
          │
          │ Result
          ▼
    core_to_ui.fifo
          │
          ▼
       UI PROCESS
          │
          ▼
        USER
```

## Example Demonstration

In the demo program, the UI sends a sequence of instructions to the Core.

```text
LOAD 10
ADD 5
PUSH
LOAD 20
POP
STORE 50
LOADM 50
```

The execution maintains the following state:

```text
LOAD 10       → ACC = 10
ADD 5         → ACC = 15
PUSH          → Stack stores 15
LOAD 20       → ACC = 20
POP           → ACC = 15
STORE 50      → Memory[50] = 15
LOADM 50      → ACC = 15
```

The results are returned to the UI, while execution information is sent separately to the Logger.

## Architecture Summary

The system separates the user interface, instruction processing, and logging into three independent processes.

Named Pipes provide the communication channels between these processes. The UI communicates with the Core using separate input and output FIFOs, while the Core sends logging information to the Logger through another FIFO.

This separation allows the simulator to demonstrate **multiprocessing, inter-process communication, instruction execution, stack management, memory management, and logging** in one system.

