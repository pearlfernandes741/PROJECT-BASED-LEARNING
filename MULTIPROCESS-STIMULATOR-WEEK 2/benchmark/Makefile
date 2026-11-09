
CC = gcc
CFLAGS = -Wall -Wextra -O2

all: benchmark standalone

benchmark: benchmark.c
	$(CC) $(CFLAGS) benchmark.c -o benchmark

standalone: standalone.c
	$(CC) $(CFLAGS) standalone.c -o standalone

run: all
	./standalone
	./benchmark

clean:
	rm -f benchmark standalone

.PHONY: all run clean
