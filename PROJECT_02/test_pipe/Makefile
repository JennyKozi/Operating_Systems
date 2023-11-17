all: prog test

prog: prog.o
	gcc -o prog prog.o

prog.o: prog.c
	gcc -c -o prog.o prog.c

test: test.o
	gcc -o test test.o

test.o: test.c
	gcc -c -o test.o test.c
	
run: test
	./test