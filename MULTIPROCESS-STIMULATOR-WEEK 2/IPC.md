# IPC Technique Used – Named Pipes (FIFOs)

## IPC Technique Used

The Multiprocess Simulator uses **Named Pipes (FIFOs)** for Inter-Process Communication.

The UI, Core, and Logger are independent processes. Named Pipes provide communication channels between these processes without requiring them to share the same process memory.

Three FIFOs are used:

| FIFO              | Communication | Used For                       |
| ----------------- | ------------- | ------------------------------ |
| `ui_to_core.fifo` | UI → Core     | Sending commands               |
| `core_to_ui.fifo` | Core → UI     | Sending results                |
| `logger_fifo`     | Core → Logger | Sending execution/log messages |

## How Named Pipes Are Used

The FIFOs are created using the Linux `mkfifo()` function.

The Core creates all three communication channels:

```text
ui_to_core.fifo
core_to_ui.fifo
logger_fifo
```

The UI uses:

```text
ui_to_core.fifo
```

to send instructions to Core and:

```text
core_to_ui.fifo
```

to receive responses.

The Core uses:

```text
logger_fifo
```

to send execution information to Logger.

The Logger reads the messages from `logger_fifo` and stores them in:

```text
simulator.log
```

## Communication Between UI and Core

The UI and Core use two separate FIFOs because communication is required in both directions.

### UI → Core

```text
UI
 │
 │ write()
 ▼
ui_to_core.fifo
 │
 │ read()
 ▼
CORE
```

The UI sends instructions such as:

```text
LOAD 10
ADD 5
PUSH
LOAD 20
POP
STORE 50
LOADM 50
```

The Core reads these instructions and executes them.

### Core → UI

```text
CORE
 │
 │ write()
 ▼
core_to_ui.fifo
 │
 │ read()
 ▼
UI
```

After executing an instruction, the Core sends information back to the UI.

This allows the UI to display the result to the user.

## Communication Between Core and Logger

The Core sends execution information through `logger_fifo`.

```text
CORE
 │
 │ write()
 ▼
logger_fifo
 │
 │ read()
 ▼
LOGGER
 │
 ▼
simulator.log
```

The Logger continuously waits for messages and writes the received information to the log file.

This keeps logging separate from the main UI interaction.

## How the Demo Uses IPC

When the user selects **Run Demo Program**, the UI sends instructions one by one.

```text
UI
 │
 ├── LOAD 10 ──────► Core
 │
 ├── ADD 5 ────────► Core
 │
 ├── PUSH ─────────► Core
 │
 ├── LOAD 20 ──────► Core
 │
 ├── POP ──────────► Core
 │
 ├── STORE 50 ─────► Core
 │
 └── LOADM 50 ─────► Core
```

The Core processes each instruction and sends the result back to the UI.

At the same time, the Core sends execution information to the Logger.

Therefore, the IPC flow during execution is:

```text
                    ┌───────────────┐
                    │      UI       │
                    └───────┬───────┘
                            │
                     Commands
                            │
                            ▼
                    ui_to_core.fifo
                            │
                            ▼
                    ┌───────────────┐
                    │     CORE      │
                    └───┬───────┬───┘
                        │       │
                 Results│       │Logs
                        │       │
                        ▼       ▼
             core_to_ui.fifo  logger_fifo
                        │       │
                        ▼       ▼
                       UI     LOGGER
                                │
                                ▼
                         simulator.log
```

## Why Named Pipes Are Suitable for This Project

Named Pipes are suitable for this simulator because the communication requirements are clearly separated.

### 1. Separate communication channels

The project uses different FIFOs for different purposes:

* `ui_to_core.fifo` for commands
* `core_to_ui.fifo` for responses
* `logger_fifo` for logging

This makes the communication paths easy to understand and manage.

### 2. Suitable for independent processes

UI, Core, and Logger run as separate processes. Named Pipes allow these processes to communicate through the Linux FIFO interface.

### 3. Simple command and response flow

The main interaction follows a simple pattern:

```text
UI → Core → UI
```

This matches the way the simulator executes instructions and returns results.

### 4. Logging is separated from user interaction

The Core can send information to Logger without sending the same information through the UI communication path.

```text
Core → Logger → simulator.log
```

This gives the project a separate logging mechanism.

### 5. Easy Linux implementation

The implementation uses standard Linux system calls such as:

```text
mkfifo()
open()
read()
write()
close()
unlink()
```

Therefore, the IPC mechanism can be implemented directly in C without requiring an additional IPC library.

## Advantages Based on Our Implementation

The following advantages apply specifically to the way Named Pipes are used in this project:

* **Clear communication paths:** Each FIFO has a specific purpose.
* **Independent processes:** UI, Core, and Logger communicate without being part of the same process.
* **Simple request-response communication:** UI sends a command and receives a response from Core.
* **Separate logging path:** Core can send execution information directly to Logger.
* **Simple data transfer:** The project mainly transfers commands, results, and log messages as text.
* **Easy process separation:** UI, processing, and logging responsibilities are separated.
* **Easy to observe and test:** The three processes can be run separately and their communication can be observed through terminal output.

## Limitations Based on Our Implementation

The limitations are also related to how this project is designed:

* **Limited to the communication pattern used by the project:** The current design mainly supports UI → Core, Core → UI, and Core → Logger communication.
* **Additional FIFO is required for another communication direction:** A new FIFO would be needed if Logger had to send information back to Core.
* **Blocking behavior:** Opening or reading from a FIFO can wait until the other process is available, which is why the programs display messages such as `Waiting for Core Process...`.
* **Text-based communication:** The current implementation sends commands and messages as character data rather than structured message objects.
* **Not designed for large amounts of data:** The simulator exchanges relatively small commands, results, and log messages.
* **FIFO lifecycle must be handled carefully:** The processes create, open, close, and remove the FIFOs during execution.

## IPC Flow in the Final System

```text
          COMMAND
UI ───────────────────► CORE
       ui_to_core.fifo

          RESULT
UI ◄─────────────────── CORE
       core_to_ui.fifo

           LOG
CORE ─────────────────► LOGGER
          logger_fifo
                           │
                           ▼
                    simulator.log
```

The Named Pipe implementation therefore provides the communication mechanism required by the three-process simulator while keeping **user interaction, instruction processing, and logging as separate processes**.
