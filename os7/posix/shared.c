#define _POSIX_C_SOURCE 200809L
#include "shared.h"

#include <sys/ipc.h>
#include <inttypes.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <string.h>



void get_shm_i(int *from, int *to_prep, int *to_send) {
    *from = sem_value(sem_open(FROM_SEM, 0));
    *to_prep = sem_value(sem_open(TO_PREP_SEM, 0));
    *to_send = sem_value(sem_open(TO_SEND_SEM, 0));
}

int sem_value(sem_t *sem) {
    int buf;
    sem_getvalue(sem, &buf);
    return buf;
}

// disgusting but I'm tired 
void init_shared(int **sh_arr, int *sh_arr_size) {
    int fd = shm_open(SHM_NAME, PERMS, MODE);
    if (fd == -1)
        error(errno, "init shm_open");
    struct stat buf;
    if (fstat(fd, &buf) == -1)
        error(errno, "init stat");
    *sh_arr_size = buf.st_size / sizeof(int);
    *sh_arr = mmap(NULL, buf.st_size, PROT, MAP_SHARED, fd, 0);
}

void error(int errsv, char *msg) {
    errno = errsv;
    perror(msg);
    kill(getpid(), SIGINT);
}

void safe_print(char *msg) {
    write(1, msg, strlen(msg));
}

void print_time() {
    struct timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);

    time_t s  = spec.tv_sec;
    long ns = spec.tv_nsec;

    printf("Time: %ld s and %ld ns since the Epoch", s, ns);
}