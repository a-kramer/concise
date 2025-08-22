CC = gcc
CFLAGS = -Wfatal-errors -O2 -march=native 

.PHONY: all test


all: concise


concise: src/main.c
	$(CC) $(CFLAGS) -o $@ $^ -lm

test: concise
	tests/test.sh
