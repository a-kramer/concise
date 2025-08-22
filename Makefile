CC = cc
CFLAGS = -Wfatal-errors -O2 -march=native 
PREFIX = /usr/local/bin
MANPREFIX = /usr/local/man/man1



.PHONY: all test


all: concise


concise: src/main.c
	$(CC) $(CFLAGS) -o $@ $^ -lm

test: concise
	tests/test.sh

install: concise
	install $^ $(PREFIX) && install man/*.1 $(MANPREFIX)  && gzip -f $(MANPREFIX)/*.1
