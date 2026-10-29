# Benchmark and Performance Analysis

## 1. Objective

To compare the standalone single-process simulator with the multi-process simulator using 1,000 addition operations and examine the performance overhead introduced by inter-process communication (IPC).

## 2. Test Setup

- **Operating System:** Ubuntu Linux (WSL)
- **Standalone Simulator:** `benchmark/standalone.c`
- **Multi-Process Simulator:** UI, Core and Logger processes
- **Number of Operations:** 1,000 additions
- **Operation:** ADD 3 5
- **Expected Result:** 8

## 3. IPC Mechanisms

The multi-process simulator uses three IPC mechanisms:

- **UNIX domain socket:** UI → Core
- **POSIX message queue:** Core → UI
- **Named pipe (FIFO):** Core → Logger

These mechanisms allow the independent processes to exchange commands, responses and logging information.

## 4. Benchmark Results

| Metric | Standalone | Multi-process |
|---|---:|---:|
| Operations | 1,000 additions | 1,000 additions |
| Final result | 8 | 8 |
| Execution time | 0.000000616 seconds | 0.551931 seconds |
| Execution time in milliseconds | Approximately 0.000616 ms | 551.931 ms |
| IPC required | No | Yes |

*The times above are the recorded results from the benchmark runs. Execution time can vary between runs and systems.*

## 5. Performance Analysis

### Standalone Simulator

The standalone program performs 1,000 additions within a single process. It does not need inter-process communication, so the measured execution time is extremely small.

### Multi-Process Simulator

The multi-process benchmark successfully completed all 1,000 addition operations. Its measured time includes the overhead associated with communicating between separate processes and other simulator execution work.

### Comparison

The standalone benchmark measures arithmetic in a single process, while the multi-process benchmark exercises the simulator and its IPC mechanisms. Therefore, these results demonstrate the difference between the two measured workloads, but they are not a perfectly controlled comparison of IPC overhead alone.

The multi-process result is slower in this test. This illustrates the potential performance cost of process separation and communication in exchange for modularity and independent process responsibilities.

## 6. Other Performance Metrics

CPU usage and memory usage have not yet been recorded quantitatively in this benchmark. They should be measured separately before making numerical claims about resource consumption.

## 7. Conclusion

The benchmark confirms that the standalone program completed 1,000 additions and produced the expected result of 8. The multi-process benchmark also completed 1,000 additions successfully with the same result.

The multi-process design introduces IPC and coordination overhead, while separating the UI, Core and Logger into independent processes. The recorded timings should be interpreted in the context of the different work performed by each benchmark.
