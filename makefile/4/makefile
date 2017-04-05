
CC=gcc
CFLAGS=-Wall -g


# gcc -Wall -g -c $^ -o $@   is the same as below
one:main.o add.o sub.o
	$(CC) $(CFLAGS) $^ -o $@

#main.o:main.c
	#gcc -Wall -g -c main.c -o main.o
#add.o:add.c add.h
	#gcc -Wall -g -c add.c -o add.o
#sub.o:sub.c sub.h
	#gcc -Wall -g -c sub.c -o sub.o
# the same as above
%.o:%.c
	gcc -Wall -g -c $^ -o $@

.PHONY:clean
clean:
	rm -f *.o one

