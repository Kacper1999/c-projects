#pragma once
#define _XOPEN_SOURCE 700

#define RECEIVER_PATH "./receiver_e"
#define SENDER_PATH "./sender_e"
#define PACKER_PATH "./packer_e"
#define PROJ_ID 'a'
#define PATH_TO_FTOK "."
#define PERMS 0666
#define SEM_NUM 5
#define FROM_I 0
#define TO_PREP_I 1
#define TO_SEND_I 2
#define FREE_I 3
#define CAN_MOD_I 4


union semun {
    int              val;    /* Value for SETVAL */
    struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
    unsigned short  *array;  /* Array for GETALL, SETALL */
    struct seminfo  *__buf;  /* Buffer for IPC_INFO
                                (Linux-specific) */
};

void update_sem(int sem_id, int sem_num, int n);
void get_shm_i(int *from, int *to_prep, int *to_send, int sem_id);
void init_shared(int *sem_id, int *shm_id, int **sh_arr, int *sh_arr_size);
void error(int errsv, char *msg);
void safe_print(char *msg);
void print_time();