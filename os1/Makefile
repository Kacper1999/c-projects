CC = gcc
CFLAGS = -Wall
BINS = lib_static_so1.a lib_shared_so1.so

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


auto_time0: auto_time_so1.c lib_so1.o
	$(CC) $(CFLAGS) -O0 -o $@ $^

runtime_lib_time0: auto_time_so1.c
	$(CC) $(CFLAGS) -O0 -o $@ $^ -L. -l_shared_so1

static_lib_time0: auto_time_so1.c
	$(CC) $(CFLAGS) -O0 -o $@ $^ -L. -l_static_so1




auto_time1: auto_time_so1.c lib_so1.o
	$(CC) $(CFLAGS) -O1 -o $@ $^

runtime_lib_time1: auto_time_so1.c
	$(CC) $(CFLAGS) -O1 -o $@ $^ -L. -l_shared_so1

static_lib_time1: auto_time_so1.c
	$(CC) $(CFLAGS) -O1 -o $@ $^ -L. -l_static_so1




auto_time2: auto_time_so1.c lib_so1.o
	$(CC) $(CFLAGS) -O2 -o $@ $^

runtime_lib_time2: auto_time_so1.c
	$(CC) $(CFLAGS) -O2 -o $@ $^ -L. -l_shared_so1

static_lib_time2: auto_time_so1.c
	$(CC) $(CFLAGS) -O2 -o $@ $^ -L. -l_static_so1	


clean:
	rm *.o $(BINS)
