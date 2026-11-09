
#!/bin/bash
set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
ROOT_DIR="$(dirname "$SCRIPT_DIR")"

cd "$ROOT_DIR"

echo "Building updated simulator..."

gcc -Wall -Wextra -O2 \
    "MEMBER 1 GERALD-UI/main.c" \
    -o ui_program -lrt

gcc -Wall -Wextra -O2 \
    "MEMBER 2 NAMAN-CORE PROCESS/core.c" \
    -o core_program -lrt

gcc -Wall -Wextra -O2 \
    logger/logger.c \
    -o logger_program

gcc -Wall -Wextra -O2 \
    benchmark/benchmark.c \
    -o benchmark/benchmark

gcc -Wall -Wextra -O2 \
    benchmark/standalone.c \
    -o benchmark/standalone

echo "Creating input for 1000 ADD operations..."

: > benchmark/benchmark_input.txt

for ((i = 0; i < 1000; i++)); do
    printf '1\n3\n5\n' >> benchmark/benchmark_input.txt
done

printf '0\n' >> benchmark/benchmark_input.txt

cd "$SCRIPT_DIR"

echo
echo "Running sequential baseline..."
./standalone

echo
echo "Running updated multi-process benchmark..."
./benchmark

echo
echo "Benchmark runs finished."

