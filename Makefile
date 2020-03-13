CC = gcc
CFLAGS = -Wall
BINS = lib_so1_test time_so1 lib_static_so1.a lib_shared_so1.so runtime_lib_test static_lib_test

all: $(BINS)


lib_so1.o: lib_so1.c lib_so1.h
	$(CC) $(CFLAGS) -c lib_so1.c

lib_shared_so1.so: lib_so1.h lib_so1.c
	$(CC) $(CFLAGS) -fPIC -shared -o $@ lib_so1.c -lc

lib_static_so1.a: lib_so1.o
	ar rcs lib_static_so1.a lib_so1.o

lib_so1_test: lib_so1_test.c lib_so1.o
	$(CC) $(CFLAGS) -o $@ $^

runtime_lib_test: lib_so1_test.c
	$(CC) $(CFLAGS) -o $@ $^ -L. -l_shared_so1

static_lib_test: lib_so1_test.c
	$(CC) $(CFLAGS) -o $@ $^ -L. -l_static_so1


lib_time.o: lib_time.c time_so1.h lib_so1.o
	$(CC) $(CFLAGS) -c lib_time.c 

time_so1: time_so1.c lib_time.o
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm *.o $(BINS)
