#include "shared.h"

#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <errno.h>


int sh_arr_size = 4;
int shm_id;
int sem_id;



void kill_all(int sig) {
    // probably not async safe
    if (shmctl(shm_id, IPC_RMID, NULL) == -1)
        error(errno, "shmctl remove"); 
    if (semctl(sem_id, 0, IPC_RMID) == -1)
        error(errno, "semct remove");

    safe_print("\nClosing...\n");    
    exit(EXIT_SUCCESS);
}

void init_sem() {
    union semun tmp;
    tmp.val = 0;
    for (int i = 0; i < SEM_NUM; i++)
        semctl(sem_id, i, SETVAL, tmp);
    tmp.val = sh_arr_size;
    semctl(sem_id, FREE_I, SETVAL, tmp);
    tmp.val = 1;
    semctl(sem_id, CAN_MOD_I, SETVAL, tmp);
}

int main(int argc, char **argv) {
    sh_arr_size = atoi(argv[1]);
    int receiver_e_num = 1;
    int packer_e_num = 1;
    int sender_e_num = 1;
    if (argc == 3) {
        receiver_e_num = atoi(argv[2]);
        packer_e_num = atoi(argv[2]);
        sender_e_num = atoi(argv[2]);
    } else if (argc == 5) {
        receiver_e_num = atoi(argv[2]);
        packer_e_num = atoi(argv[3]);
        sender_e_num = atoi(argv[4]);
    }
    // handle ^C
    struct sigaction new_handle;
    new_handle.sa_handler = kill_all;
    sigaction(SIGINT, &new_handle, NULL);

    // get keys
    key_t key = ftok(PATH_TO_FTOK, PROJ_ID);

    // SHared Memory ID
    if ((shm_id = shmget(key, sizeof(int) * sh_arr_size, IPC_CREAT | IPC_EXCL | PERMS)) == -1) 
        error(errno, "shmget");
    // SEMaphore ID
    sem_id = semget(key, SEM_NUM, IPC_CREAT | IPC_EXCL | PERMS);
    if (sem_id == -1)
        error(errno, "semget");
    init_sem();

    // SHared ARRay
    int *sh_arr = shmat(shm_id, NULL, 0);
    if (sh_arr == (int *) -1)
        error(errno, "shmat");

    // calling workers
    for (int i = 0; i < receiver_e_num; i++)
        if (fork() == 0)
            if (execl(RECEIVER_PATH, RECEIVER_PATH, NULL) == -1)
                error(errno, "execl receiver");
    for (int i = 0; i < packer_e_num; i++)    
        if (fork() == 0)
            if (execl(PACKER_PATH, PACKER_PATH, NULL))
                error(errno, "excl packer");
    for (int i = 0; i < sender_e_num; i++)
        if (fork() == 0) 
            if (execl(SENDER_PATH, SENDER_PATH, NULL))
            error(errno, "excl sender");

    while (true);

    return 0;
}