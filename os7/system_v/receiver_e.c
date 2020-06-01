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

void got_order() {
    update_sem(sem_id, CAN_MOD_I, -1); // we don't want any other process to get in our way
    if (semctl(sem_id, FREE_I, GETVAL) <= 0) {
        update_sem(sem_id, CAN_MOD_I, 1);
        return;
    }
    update_sem(sem_id, FREE_I, -1);

    int n = rand() % 10 + 1;

    int from, to_prep, to_send;
    get_shm_i(&from, &to_prep, &to_send, sem_id);

    sh_arr[(from + to_prep + to_send) % sh_arr_size] = n;
    printf("PID: %d\t", getpid());
    print_time();
    printf("\nAdded: %d. Orders to prepare: %d. Orders to send: %d\n", n, ++to_prep, to_send);

    update_sem(sem_id, TO_PREP_I, 1); // we added record so there is one more to prepare
    update_sem(sem_id, CAN_MOD_I, 1); // we are done so other process can modify arr
}

int main(int argc, char** argv) {
    srand((unsigned int)time(NULL));
    init_shared(&sem_id, &shm_id, &sh_arr, &sh_arr_size);
    
    while(true) {
        got_order();
        sleep(2);
    }
    return 0;
}