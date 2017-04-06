.PHONY:one clean print

SRC=$(wildcard *.c)
SRCCXX=$(wildcard *.cpp)
BIN=$(SRC:%.c=%)
BINCXX=$(SRCCPP:%.cpp=%)


CC=gcc
CXX=g++
CFLAGES=-Wall
CXXFLAGES=-Wall -std=c++11

#phony target
one:$(BIN) $(BINCXX)

#only for this DIR
$(BIN):%:%.c
	$(CC) $(CFLAGES) $^ -o $@ -lmysqlclient


clean:
	rm -f $(BIN) $(BINCXX)

print:
	@echo $(SRC) $(BIN)
