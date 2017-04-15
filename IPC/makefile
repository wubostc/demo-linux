
.PHONY:clean all

CC=gcc
CFLAGES=-Wall -g
SRC=$(wildcard *.c)
BIN=$(SRC:%.c=%)

all:$(BIN)


$(BIN):%:%.c
	$(CC) $(CFLAGES) $^ -o $@

clean:
	rm -f $(BIN)
