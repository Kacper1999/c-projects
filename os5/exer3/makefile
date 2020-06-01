CC = gcc -Wall

all: test

test: test.c producer.c consumer.c
		$(CC) -o test test.c
		$(CC) -o consumer consumer.c
		$(CC) -o producer producer.c
		./test fifo a.txt b.txt c.txt d.txt e.txt 1 out.txt
		rm -f fifo

clear:
	rm -f fifo