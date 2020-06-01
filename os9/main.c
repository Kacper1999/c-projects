#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>

#include "shared.h"
#include "barber.h"
#include "client.h"


int main(int argc, char **argv) {
    srand(time(NULL));
    seats_num = atoi(argv[1]);
    int init_clients_num = atoi(argv[2]);
    
    rem_clients = init_clients_num;
    is_barber_sleeping = true;
    curr_shaved = FREE_SEAT;
    sem_init(&free_seats, 0, seats_num);
    pthread_mutex_init(&mtx, NULL);
    pthread_cond_init(&cond, NULL);

    pthread_t *threads = malloc(sizeof(pthread_t) * (init_clients_num + 1));  // +1 because barber will be first
    pthread_create(&threads[0], NULL, barber_fun, (void *)0);
    for (long i = 1; i < init_clients_num + 1; i++) {
        sleep(rand_int(S_LB, S_UB)); // we can comment this line out to be sure there won't be free place at
        pthread_create(&threads[i], NULL, client_fun, (void *)i);
    }

    for (int i = 0; i < init_clients_num + 1; i++) {
        long j;
        pthread_join(threads[i], (void *)(&j));
        printf("thread %d returns: %ld\n", i, j);  // just to be sure
    }
    
    return 0;
}