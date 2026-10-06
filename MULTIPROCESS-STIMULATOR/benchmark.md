# Benchmark

## Purpose

The purpose of this benchmark is to evaluate the performance and execution of the Multi-Process Simulator and compare it with a Standalone Single-Process Simulator.

## Test Setup

* **Operating System:** Ubuntu Linux
* **Standalone Simulator:** Single-process implementation
* **Multi-Process Simulator:** UI Process, Core Process and Logger Process
* **IPC Technique:** Named Pipes (FIFO)
* **Test Program:** Demo Program
* **Number of Instructions:** 7

## Parameters Measured

The following performance metrics were considered:

* Execution Time
* CPU Usage
* Memory Usage
* IPC Overhead
* Number of Instructions Executed

## IPC Methods Tested

The Multi-Process Simulator uses **Named Pipes (FIFO)** for communication between the UI, Core and Logger processes.

The communication flow is:

**UI Process → Core Process → Logger Process**

The FIFO channels used are:

* `ui_to_core.fifo`
* `core_to_ui.fifo`
* `logger_fifo`

## Benchmark Results

### Multi-Process Simulator

| Parameter              |             Result |
| ---------------------- | -----------------: |
| Execution Time         |   **2.01 seconds** |
| User CPU Time          |   **0.97 seconds** |
| Instructions Executed  |              **7** |
| Final Program Counter  |              **7** |
| Final Accumulator      |             **15** |
| Final Stack Top        |             **-1** |
| UI → Core Messages     |              **7** |
| Core → UI Messages     |              **7** |
| Core → Logger Messages |              **7** |
| Total IPC Messages     |             **21** |

## Execution Results

The following 7 instructions were successfully executed:

1. `LOAD 10`
2. `ADD 5`
3. `PUSH`
4. `LOAD 20`
5. `POP`
6. `STORE 50`
7. `LOADM 50`

The final results were:

* **Program Counter = 7**
* **Accumulator = 15**
* **Stack Top = -1**
* **Memory[50] = 15**

The Logger Process successfully received the execution messages and stored them in `simulator.log`.

## Standalone Single-Process vs Multi-Process Simulator

| Parameter            | Standalone Single-Process Simulator | Multi-Process Simulator          |
| -------------------- | ----------------------------------- | -------------------------------- |
| Architecture         | Single process                      | Multiple independent processes   |
| Execution Time       | Lower                               | **0.000085 seconds**               |
| IPC Overhead         | None                                | Present                          |
| CPU Usage            | Lower                               | Higher due to multiple processes |
| Memory Usage         | Lower                               | Higher due to multiple processes |
| Process Independence | Low                                 | High                             |

## Conclusion

The benchmark demonstrates that the Multi-Process Simulator successfully executes the required instructions and provides communication between independent UI, Core and Logger processes using Named Pipes.

The Standalone Single-Process Simulator has lower overhead because all components execute within one process and do not require IPC.

The Multi-Process Simulator introduces IPC and multiple-process overhead, which can increase execution and resource usage. However, it provides better process separation, modularity, fault isolation and scalability.

For the current project, the Multi-Process Simulator successfully achieves the required functionality using Named Pipes (FIFO).
