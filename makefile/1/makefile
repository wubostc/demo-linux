#Target ...:dependencies...
	#command
	#...


#Target:程序生成的文件，或者要指向的动作，如clean
#dependencies:目标文件依赖的文件
#command:make执行的动作以tab字符开始
#dependencies 中文件更新时，执行command



# define a var
# $(OBJS1)  is the same as <main.o add.o sub.o>
OBJS1=main.o add.o sub.o


# $@ name of target
# $^ all the file names that depends on the rule
#one:main.o add.o sub.o is the same as below
one:$(OBJS1)
	gcc -Wall -g $^ -o $@
#gcc -Wall -g main.o add.o sub.o -o one is the same as above


# -c only complie without link
# -o complied object
add.o:add.c sub.h
	gcc -Wall -g -c add.c -o add.o


# $< first name of file that depends on the rule
main.o:main.c
	gcc -Wall -g -c $< -o $@
#gcc -Wall -g -c main.c -o main.o is the same as above


sub.o:sub.c sub.h
	gcc -Wall -g -c sub.c -o sub.o
	@echo 1 - $@ [OK]
	@echo 3 - $< [OK]
	@echo 2 - $^ [OK]


# clean is only a command
.PHONY:clean

# clean is a PHONY target
# u can use the <make clean> to clean the
# main.o, add.o, sub.o, one on the bash
clean:
	rm -f $(OBJS1) one

