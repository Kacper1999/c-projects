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

int sem_id;
int shm_id;
int *sh_arr;
int sh_arr_size;

void pack() {
    update_sem(sem_id, CAN_MOD_I, -1); // we don't want any other process to get in our way
    if (semctl(sem_id, TO_SEND_I, GETVAL) <= 0) {
        update_sem(sem_id, CAN_MOD_I, 1); // we need at least one order to packed and ready to send
        return;
    }
    update_sem(sem_id, TO_SEND_I, -1);

    int from, to_prep, to_send;
    get_shm_i(&from, &to_prep, &to_send, sem_id);

    int n = sh_arr[from] * 3;
    printf("PID: %d\t", getpid());
    print_time();
    printf("\nSend: %d. Orders to prepare: %d. Orders to send: %d\n", n, to_prep, to_send);

    update_sem(sem_id, FREE_I, 1); // we send so we can dont need to store this order anymore
    int update_from = (from + 1 == sh_arr_size) ? -(sh_arr_size -1) : 1; // modulo
    update_sem(sem_id, FROM_I, update_from); // we need to remeber wich is our "start" index in shared memory
    update_sem(sem_id, CAN_MOD_I, 1); // we are done so other process can modify arr
}

int main(int argc, char** argv) {
    init_shared(&sem_id, &shm_id, &sh_arr, &sh_arr_size);
    
    while(true) {
        pack();
        sleep(2);
    }
    return 0;
}