#include <stdio.h>
#include <unistd.h>

#include "shared.h"

void occupied() {
    printf("Occupied: %lu\n", pthread_self());
    sleep(rand_int(S_LB, S_UB));
}

void *client_fun(void *i) {
    while (sem_trywait(&free_seats) == -1) 
        occupied();
    
    sem_wait(&is_barber_busy);

    int tmp;
    sem_getvalue(&free_seats, &tmp);
    printf("%lu got his place, %d people waiting in line\n", pthread_self(), seats_num - tmp);

    pthread_mutex_lock(&mtx);
    if (is_barber_sleeping) {
        printf("Waking barber... %lu\n", pthread_self());
        is_barber_sleeping = false;
    }
    curr_shaved = pthread_self();
    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&mtx);

    pthread_exit(i);
}