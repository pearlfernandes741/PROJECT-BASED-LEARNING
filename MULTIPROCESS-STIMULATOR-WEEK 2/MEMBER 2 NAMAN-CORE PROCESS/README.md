# Core Process

## Description

This module implements the core process of the multi-process simulator. It handles CPU execution, memory, stack, and queue operations.

## IPC Method

POSIX Named Pipes (FIFO) are used for interprocess communication.

The Core Process communicates with the UI Process and Logging Process using named pipes.

## Files

- `core.c` - Contains the core process implementation.
- `README.md` - Documentation for the Core Process.
- `test_cases.txt` - Contains test cases for the Core Process.

## Working

1. The Core Process receives instructions from the UI Process through a FIFO.
2. It executes the instructions using the CPU simulation.
3. It manages memory operations such as storing and loading values.
4. It manages stack operations such as PUSH and POP.
5. It processes instructions using the queue.
6. The execution result is sent back to the UI Process.
7. Execution information is sent to the Logging Process.

## Supported Instructions

- `LOAD` - Loads a value into the accumulator.
- `ADD` - Adds a value to the accumulator.
- `SUB` - Subtracts a value from the accumulator.
- `STORE` - Stores the accumulator value in memory.
- `LOADM` - Loads a value from memory into the accumulator.
- `PUSH` - Pushes the accumulator value onto the stack.
- `POP` - Removes a value from the stack.

## Compilation

```bash
gcc core.c -o core

## execution

./core