CC = gcc
CFLAGS = -Wall
BINS = client server test
SH = shared.c
LIBS = -pthread
all: $(BINS)

client: client.c
	$(CC) $(CFLAGS) -o $@ $(SH) $^ $(LIBS)

server: server.c
	$(CC) $(CFLAGS) -o $@ $(SH) $^ $(LIBS)

test: test.c
	$(CC) $(CFLAGS) -o $@ $(SH) $^ $(LIBS)

clean:
	rm $(BINS)
