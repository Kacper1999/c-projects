CC = gcc
CFLAGS = -Wall
BINS = sender catcher

all: $(BINS)

sender: sender.c
	$(CC) $(CFLAGS) -o $@ $^

catcher: catcher.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm $(BINS)
