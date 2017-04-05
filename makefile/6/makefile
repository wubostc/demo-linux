.PHONY:all one clean foo


#wildcard: replace the file with the symbol *.c mode
SRC1=$(wildcard *.c)

#e.g.    a.c b.c -> a b
BIN1=$(SRC1:%.c=%)

#e.g.    a.c b.c -> a.o b.o
OBJ1=$(SRC1:%.c=%.o)


#SRC1=main.c add.c sub.c
#BIN1=main add sub
#OBJ1=main.o add.o sub.o
#ECHO:
#@echo SRC1=$(SRC1)
#@echo BIN1=$(BIN1)
#@echo OBJ1=$(OBJ1)

all:one foo

one:$(OBJ1)
	gcc -Wall -g $^ -o $@

#$(OBJ1):%.o   is target            main.o -> add.o -> sub.o
#           :%.c   is dependencies  main.c -> add.c -> sub.c
$(OBJ1):%.o:%.c
	gcc -Wall -g -c $^ -o $@

#automatic cleaning after compile
foo:
	rm -f $(OBJ1)

clean:
	rm -f  $(OBJ1) one

