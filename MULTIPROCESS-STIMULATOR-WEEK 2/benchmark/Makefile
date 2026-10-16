CC = gcc
CFLAGS = -Wall -Wextra -O2

all: benchmark standalone

benchmark: benchmark.c
	$(CC) $(CFLAGS) benchmark.c -o benchmark

standalone: standalone.c
	$(CC) $(CFLAGS) standalone.c -o standalone

run-standalone: standalone
	./standalone

run-benchmark: benchmark
	./benchmark

clean:
	rm -f benchmark standalone
