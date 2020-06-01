#include "shared.h"

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <time.h>
#include <stdbool.h>

#include <semaphore.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

int sh_arr_size;
int *sh_arr;
int shm_fd;
sem_t *from_sem;
sem_t *to_prep_sem;
sem_t *to_send_sem;
sem_t *free_sem;
sem_t *can_mod_sem;

void pack() {
    sem_wait(can_mod_sem);
    if (sem_value(to_prep_sem) <= 0) {
        sem_post(can_mod_sem);
        return;
    }
    sem_wait(to_prep_sem);

    int from, to_prep, to_send;
    get_shm_i(&from, &to_prep, &to_send);

    int n = sh_arr[(from + to_send) % sh_arr_size] * 2;
    printf("PID: %d\t", getpid());
    print_time();
    printf("\nPacked: %d. Orders to prepare: %d. Orders to send: %d\n", n, to_prep, ++to_send);

    sem_post(to_send_sem);
    sem_post(can_mod_sem);
}

int main(int argc, char** argv) {
    init_shared(&sh_arr, &sh_arr_size);

    from_sem = sem_open(FROM_SEM, 0);
    to_prep_sem = sem_open(TO_PREP_SEM, 0);
    to_send_sem = sem_open(TO_SEND_SEM, 0);
    free_sem = sem_open(FREE_SEM, 0);
    can_mod_sem = sem_open(CAN_MOD_SEM, 0);
    
    while(true) {
        pack();
        sleep(2);
    }
    return 0;
}