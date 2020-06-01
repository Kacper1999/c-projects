#pragma once
#define _XOPEN_SOURCE 700

#include <semaphore.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

#define RECEIVER_PATH "./receiver_e"
#define SENDER_PATH "./sender_e"
#define PACKER_PATH "./packer_e"

#define SEM_NUM 5
#define FROM_SEM "from_sem"
#define TO_PREP_SEM "to_prep_sem"
#define TO_SEND_SEM "to_send_sem"
#define FREE_SEM "free_sem"
#define CAN_MOD_SEM "can_mod_sem"

#define SHM_NAME "shm_name"

#define MODE S_IRUSR | S_IWUSR
#define PERMS O_RDWR
#define CREATE_FLAGS O_CREAT | O_EXCL | PERMS
#define PROT PROT_READ | PROT_WRITE

#define PRINTY_BOI "PRINTY BOI\n"

void get_shm_i(int *from, int *to_prep, int *to_send);
void init_shared(int **sh_arr, int *sh_arr_size);
int sem_value(sem_t *sem);
void error(int errsv, char *msg);
void safe_print(char *msg);
void print_time();