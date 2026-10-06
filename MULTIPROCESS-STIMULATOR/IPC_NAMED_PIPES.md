# IPC Technique: Named Pipes (FIFOs)

The processes are independent programs (not parent and child), so they cannot share anonymous pipes. A **named pipe (FIFO)** is created using `mkfifo()` and appears as a special file in the filesystem. Any process that opens the same FIFO can communicate through it.

## FIFO Communication

| FIFO              | Direction     | Writer | Reader | Content                                        |
| ----------------- | ------------- | ------ | ------ | ---------------------------------------------- |
| `ui_to_core.fifo` | UI → Core     | UI     | Core   | Instruction text, e.g. `ADD 5\n`               |
| `core_to_ui.fifo` | Core → UI     | Core   | UI     | Result text, e.g. `Executed ADD 5 \| ACC = 15` |
| `logger_fifo`     | Core → Logger | Core   | Logger | Log lines                                      |

A FIFO is **one-directional**, so the UI and Core conversation needs **two FIFOs**, one for each direction.

## How It Works

### 1. Creation

`mkfifo(name, 0666)` creates the pipe. The code ignores `EEXIST`, so it does not matter which process creates it first.

### 2. Blocking Open (Rendezvous)

`open()` on a FIFO blocks until the other end is also opened. This makes the processes wait for each other.

* `Waiting for UI...`
* `UI connected.`
* `Logger connected.`

### 3. Communication

`write()` sends bytes and `read()` receives them.

The UI/Core exchange follows a strict **request and response** pattern:

1. UI writes a command.
2. Core reads and executes the command.
3. Core sends the result.
4. UI reads and displays the result.

### 4. Teardown

On `EXIT`, descriptors are closed using `close()` and the FIFO files are deleted using `unlink()`.

## Sequence of the Demo Program

```text
UI                         Core                       Logger
 |                           |                           |
 |       Connect             |                           |
 |-------------------------->|                           |
 |                           |-------------------------->|
 |                           |       Connect Logger      |
 |                           |                           |
 |   LOAD 10                 |                           |
 |-------------------------->|                           |
 |                           | Execute LOAD 10           |
 |                           | ACC = 10                  |
 |                           |-------------------------->|
 |                           |                           | Log message
 |<--------------------------|                           |
 |   Executed LOAD 10        |                           |
 |                           |                           |
 |   STATUS                  |                           |
 |-------------------------->|                           |
 |                           | Check PC, ACC, Stack      |
 |<--------------------------|                           |
 |   PC, ACC, Stack Top      |                           |
 |                           |                           |
 |   EXIT                    |                           |
 |-------------------------->|                           |
 |                           | Core shuts down           |
```

## Communication Pattern

### UI and Core

* **Client-server communication**
* **Synchronous communication**
* **Lockstep request/response**
* Uses **two one-way FIFOs**

### Core and Logger

* **One-way communication**
* **Fire-and-forget logging**
* Core acts as the **producer**
* Logger acts as the **sink**

# Build and Run

## Compile the Programs

```bash
gcc ui.c -o ui_program
gcc core.c -o core_program
gcc logger.c -o logger_program
```

## Create the FIFOs

Create the pipes first so the start order does not matter:

```bash
mkfifo ui_to_core.fifo core_to_ui.fifo logger_fifo 2>/dev/null
```

## Run the Programs

Open **three terminals**.

### Terminal 1 – Logger

```bash
./logger_program
```

### Terminal 2 – Core

```bash
./core_program
```

### Terminal 3 – UI

```bash
./ui_program
```

If Core is started before any FIFO exists, it can fail with:

```text
UI FIFO: No such file or directory
```

Creating the FIFOs first avoids this problem.
