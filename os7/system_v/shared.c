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


void update_sem(int sem_id, int sem_num, int sem_op) {
    struct sembuf op;
    op.sem_num = sem_num;
    op.sem_op = sem_op;
    op.sem_flg = 0;
    if (semop(sem_id, &op, 1) == -1)
        error(errno, "semop");
}

void get_shm_i(int *from, int *to_prep, int *to_send, int sem_id) {
    *from = semctl(sem_id, FROM_I, GETVAL);
    *to_prep = semctl(sem_id, TO_PREP_I, GETVAL);
    *to_send = semctl(sem_id, TO_SEND_I, GETVAL);
}

void init_shared(int *sem_id, int *shm_id, int **sh_arr, int *sh_arr_size) {
    key_t key = ftok(PATH_TO_FTOK, PROJ_ID);
    *sem_id = semget(key, 0, 0);
    if (*sem_id == -1)
        error(errno, "init semget");
    *shm_id = shmget(key, 0, 0);
    if (*shm_id == -1)
        error(errno, "init shmget");
    *sh_arr = shmat(*shm_id, NULL, 0);
    if (*sh_arr == (int *)-1)
        error(errno, "init shmat");
    struct shmid_ds buf;
    if (shmctl(*shm_id, IPC_STAT, &buf))
        error(errno, "shmctl IPC_STAT");
    else
        *sh_arr_size = buf.shm_segsz;
}

void error(int errsv, char *msg) {
    errno = errsv;
    perror(msg);
    exit(EXIT_FAILURE);
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