# Benchmark

## Purpose

The benchmark is used to evaluate the performance of the Multiprocess Simulator and study the communication performance of the IPC technique used in the project.

The current implementation uses **Named Pipes (FIFOs)** for communication between the UI, Core, and Logger processes.

The benchmark can later be used to compare Named Pipes with other IPC techniques using the same simulator workload.

---

## Test Setup

* **Operating System:** Linux
* **Programming Language:** C
* **Compiler:** GCC
* **Number of Processes:** 3
* **Processes:** UI, Core, Logger
* **IPC Technique Used:** Named Pipes (FIFO)
* **Main Test:** Demo program execution
* **Instructions in Demo:** 7
* **Log Output:** `simulator.log`

### Demo Instructions

The benchmark uses the simulator's demo sequence:

```text
LOAD 10
ADD 5
PUSH
LOAD 20
POP
STORE 50
LOADM 50
```

These instructions are sent from the UI to the Core one by one.

The Core processes each instruction and sends the result back to the UI. The Core also sends execution information to the Logger.

---

## IPC Communication Paths

The simulator uses three Named Pipes:

| FIFO              | Direction     | Purpose                                        |
| ----------------- | ------------- | ---------------------------------------------- |
| `ui_to_core.fifo` | UI → Core     | Sends instructions and commands                |
| `core_to_ui.fifo` | Core → UI     | Sends execution results and status information |
| `logger_fifo`     | Core → Logger | Sends execution information for logging        |

The communication flow is:

```text
UI
 |
 | ui_to_core.fifo
 v
Core
 |
 +---- core_to_ui.fifo ----> UI
 |
 +---- logger_fifo --------> Logger
                              |
                              v
                        simulator.log
```

---

## Parameters Measured

The following parameters will be considered during benchmarking:

* **Execution Time** – Time required to execute the complete test sequence.
* **Communication Time** – Time required for messages to travel between the processes through the FIFOs.
* **CPU Usage** – CPU resources used by the simulator processes during execution.
* **Memory Usage** – Memory consumed by the UI, Core, and Logger processes.
* **Number of Messages** – Number of instructions, responses, and log messages transferred during the test.

---

## Current Test Workload

The demo program contains **7 instructions**.

| Step | Instruction | Expected Simulator State    |
| ---- | ----------- | --------------------------- |
| 1    | `LOAD 10`   | ACC = 10                    |
| 2    | `ADD 5`     | ACC = 15                    |
| 3    | `PUSH`      | 15 is pushed onto the stack |
| 4    | `LOAD 20`   | ACC = 20                    |
| 5    | `POP`       | ACC = 15                    |
| 6    | `STORE 50`  | Memory[50] = 15             |
| 7    | `LOADM 50`  | ACC = 15                    |

This workload is used as the common test case so that future IPC methods can be compared under the same conditions.

---

## Benchmark Results

| IPC Method         | Execution Time | Communication Time | CPU Usage | Memory Usage | Remarks                                         |
| ------------------ | -------------: | -----------------: | --------: | -----------: | ----------------------------------------------- |
| Pipes              |              — |                  — |         — |            — | Not implemented in current simulator            |
| Named Pipes (FIFO) |              — |                  — |         — |            — | Current IPC implementation; measurement pending |
| Message Queues     |              — |                  — |         — |            — | Not implemented in current simulator            |
| Shared Memory      |              — |                  — |         — |            — | Not implemented in current simulator            |
| Sockets            |              — |                  — |         — |            — | Not implemented in current simulator            |
| Signals            |              — |                  — |         — |            — | Not implemented in current simulator            |

---

## Named Pipe Benchmark

The current simulator uses Named Pipes as its IPC mechanism.

During the test, communication takes place through:

```text
ui_to_core.fifo
core_to_ui.fifo
logger_fifo
```

The benchmark measurements for the current implementation will be added after performing multiple runs under the same system conditions.

| Run     | Execution Time | Communication Time | CPU Usage | Memory Usage |
| ------- | -------------: | -----------------: | --------: | -----------: |
| Run 1   |              — |                  — |         — |            — |
| Run 2   |              — |                  — |         — |            — |
| Run 3   |              — |                  — |         — |            — |
| Average |              — |                  — |         — |            — |

---

## Number of Processes

The current implementation consists of three independent processes:

```text
UI Process
    |
    v
Core Process
    |
    v
Logger Process
```

The UI handles user interaction, the Core performs instruction execution, and the Logger records execution information.

---

## Test Conditions

For meaningful comparison, the same conditions should be maintained for every benchmark run:

* Same Linux environment
* Same simulator workload
* Same number of instructions
* Same number of processes
* Same compiler and compilation settings
* Same test sequence
* Multiple runs for each IPC technique

The average of multiple runs can be used for the final comparison.

---

## Final Comparison

After measurements are collected, the benchmark results can be used to compare the IPC techniques based on:

* Execution speed
* Communication overhead
* CPU consumption
* Memory consumption
* Data/message transfer efficiency

The final numerical results will be added after the benchmark tests are performed.
