#!/bin/bash

echo "========================================"
echo "       MULTI-PROCESS BENCHMARK"
echo "========================================"

echo "2" > benchmark_input.txt
echo "4" >> benchmark_input.txt

echo ""
echo "Running Standalone Simulator..."
echo "----------------------------------------"

/usr/bin/time -f "User CPU Time: %U seconds" ./standalone

echo ""
echo "Running Multi-Process Simulator..."
echo "----------------------------------------"

./benchmark

echo ""
echo "Benchmark completed."

rm -f benchmark_input.txt
