CC = gcc
CFLAGS = -Wfatal-errors -O2 -march=native 

.PHONY: all test


all: concise


concise: main.c
	$(CC) $(CFLAGS) -o $@ $^ -lm

test: concise
	./test.sh
