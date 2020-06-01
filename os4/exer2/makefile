CC = gcc
CFLAGS = -Wall
BINS = main main_exec

all: $(BINS)

main_exec: main_exec.c
	$(CC) $(CFLAGS) -o $@ $^

 main: main.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm $(BINS)
