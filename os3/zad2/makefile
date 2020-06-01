CC = gcc 
all: test

test: main.c child.c
		$(CC) -o main main.c
		$(CC) -o child child.c
		./main lists 10 10 0

gen_test: gen.c
		$(CC) -o gen gen.c
		./gen 1 3 5

unit_test: main.c child.c unit_test.c
		$(CC) -o main main.c
		$(CC) -o child child.c
		$(CC) -o unit_test unit_test.c
		./unit_test