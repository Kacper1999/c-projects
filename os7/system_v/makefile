CC = gcc
CFLAGS = -Wall
BINS = main receiver_e packer_e sender_e

all: $(BINS)

main: main.c
	$(CC) $(CFLAGS) -o $@ shared.c $^

receiver_e: receiver_e.c
	$(CC) $(CFLAGS) -o $@ shared.c $^

packer_e: packer_e.c
	$(CC) $(CFLAGS) -o $@ shared.c $^

sender_e: sender_e.c
	$(CC) $(CFLAGS) -o $@ shared.c $^

clean:
	rm $(BINS)
