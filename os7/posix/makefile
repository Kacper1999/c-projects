CC = gcc
CFLAGS = -Wall
BINS = main receiver_e packer_e sender_e
SH = shared.c
LIBS = -lpthread -lrt

all: $(BINS)

main: main.c
	$(CC) $(CFLAGS) -o $@ $(SH) $^ $(LIBS)

receiver_e: receiver_e.c
	$(CC) $(CFLAGS) -o $@ $(SH) $^ $(LIBS)

packer_e: packer_e.c
	$(CC) $(CFLAGS) -o $@ $(SH) $^ $(LIBS)

sender_e: sender_e.c
	$(CC) $(CFLAGS) -o $@ $(SH) $^ $(LIBS)

clean:
	rm $(BINS)
