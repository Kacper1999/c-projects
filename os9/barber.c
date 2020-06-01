#include <stdio.h>
#include <unistd.h>

#include "shared.h"

void shave() {
    pthread_mutex_lock(&mtx);
    while (curr_shaved == FREE_SEAT) 
        pthread_cond_wait(&cond, &mtx);
    pthread_mutex_unlock(&mtx);
    
    sleep(rand_int(S_LB, S_UB));

    int tmp;
    sem_getvalue(&free_seats, &tmp);
    
    printf("Shaved client %lu clients_waiting: %d clients_rem: %d\n", curr_shaved, seats_num - tmp - 1, rem_clients - 1);
    curr_shaved = FREE_SEAT;  // no need to make any kind of a lock
    rem_clients--;
    sem_post(&free_seats);
    sem_post(&is_barber_busy);
}

void barber_sleeps() {
    pthread_mutex_lock(&mtx);
    is_barber_sleeping = true;
    printf("Barber sleeps...\n");
    while (curr_shaved == FREE_SEAT) 
        pthread_cond_wait(&cond, &mtx);
    pthread_mutex_unlock(&mtx);
}

void *barber_fun(void *i) {
    sem_init(&is_barber_busy, 0, 1); 
    while (rem_clients != 0) {
        int tmp;
        sem_getvalue(&free_seats, &tmp);
        if (tmp == seats_num)
            barber_sleeps();
        else
            shave();
    }
    pthread_exit(i);
}

