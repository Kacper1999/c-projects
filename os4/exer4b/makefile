CC = gcc
CFLAGS = -Wall
BINS = sender catcher test

all: $(BINS)

test: test.c
	$(CC) $(CFLAGS) -o $@ $^

sender: sender.c
	$(CC) $(CFLAGS) -o $@ $^

catcher: catcher.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm $(BINS)
