CC = gcc -Wall -std=c11 -ggdb

clean:
	rm -f -r .tmp files supp main lista

helper:
	rm -f supp
	$(CC) supp.c -o supp

test:
	make clean
	$(CC) supp.c -o supp 
	$(CC) main.c -o main
	./supp create 7 14 5
	./main lista 6 3 2