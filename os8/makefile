CC = gcc
CFLAGS = -Wall
BINS = main
LIBS = -pthread
IMG_DIR = ./images
THREAD_NUMS = 1 2 4 8
MODES = sign block int
all: $(BINS)

main: main.c
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

test:
	make main
	for file_p in $(IMG_DIR)/* ; do \
		for thread_num in $(THREAD_NUMS) ; do \
			for mode in $(MODES) ; do \
				echo $$file_p $$mode $$thread_num 0 0; \
				./main $$file_p $$mode $$thread_num 0 0; \
			done ; \
		done ; \
	done

tmp1:
	make main
	./main  ./images/apollonian_gasket.ascii.pgm int 1 0 0

tmp4:
	make main
	./main  ./images/apollonian_gasket.ascii.pgm int 4 0 0

tmp8:
	make main
	./main  ./images/apollonian_gasket.ascii.pgm int 8 0 0

clean:
	rm $(BINS)
