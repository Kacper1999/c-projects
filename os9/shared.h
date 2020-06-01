#pragma once
#define _XOPEN_SOURCE 700
#include <pthread.h>
#include <stdbool.h>
#include <semaphore.h>

// Sleep Lower/Upper Bound
#define S_LB 1
#define S_UB 4
#define FREE_SEAT -1

int seats_num;
int rem_clients;
sem_t free_seats;
sem_t is_barber_busy;
bool is_barber_sleeping;
unsigned long curr_shaved;
pthread_mutex_t mtx;
pthread_cond_t cond;

int rand_int(int a, int b);