CC = gcc
CFLAGS = -Wall
BINS = main
LIBS = -pthread
SH = shared.c barber.c client.c 
all: $(BINS)

main: main.c
	$(CC) $(CFLAGS) -o $@ $(SH) $^ $(LIBS)

clean:
	rm $(BINS)
