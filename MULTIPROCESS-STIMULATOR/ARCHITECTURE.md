# Multiprocess Simulator – Architecture

## Overall Architecture

```text
                         MULTIPROCESS SIMULATOR
                                  │
                 ┌────────────────┼────────────────┐
                 │                │                │
                 ▼                ▼                ▼
            ┌─────────┐     ┌─────────────┐   ┌───────────┐
            │   UI    │     │    CORE     │   │  LOGGER   │
            │ Process │     │   Process   │   │  Process  │
            └────┬────┘     └──────┬──────┘   └─────▲─────┘
                 │                 │                │
                 │                 │                │
       ui_to_core.fifo             │         logger_fifo
                 │                 │                │
                 └──────────────►  │                │
                                   │                │
                         core_to_ui.fifo            │
                                   │                │
                 ┌─────────────────┘                │
                 │                                  │
                 └──────────────────────────────────┘
```

## Components

### UI Process

* Accepts commands from the user.
* Sends commands to the Core process.
* Receives execution results from Core.
* Displays the results to the user.

### Core Process

* Acts as the main processing unit.
* Receives commands from UI.
* Executes the required operation.
* Sends results back to UI.
* Sends log messages to the Logger.

### Logger Process

* Receives log messages from Core.
* Records important events and operations.
* Works independently from UI and Core.

## Communication

The processes communicate using **Named Pipes (FIFOs)**.

| Communication | FIFO              | Purpose            |
| ------------- | ----------------- | ------------------ |
| UI → Core     | `ui_to_core.fifo` | Sends commands     |
| Core → UI     | `core_to_ui.fifo` | Sends results      |
| Core → Logger | `logger_fifo`     | Sends log messages |

## Working Flow

```text
User
  │
  ▼
 UI Process
  │
  │ Command
  ▼
ui_to_core.fifo
  │
  ▼
Core Process
  │
  ├──────────────► core_to_ui.fifo ──────► UI
  │                    Result
  │
  └──────────────► logger_fifo ──────────► Logger
                       Log Message
```

## Architecture Summary

The system consists of **three independent processes: UI, Core, and Logger**. Named Pipes provide communication between these processes. UI and Core use a **request-response communication pattern**, while Core sends log information to Logger using **one-way communication**. This architecture allows each process to work independently while communicating through well-defined IPC channels.
