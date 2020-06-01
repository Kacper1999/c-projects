CC = gcc
CFLAGS = -Wall
BINS = test exe tmp
LIBS = -lpthread -lrt

all: $(BINS)

test: test.c
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

exe: exe.c
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

tmp: tmp.c
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

clean:
	rm $(BINS)
