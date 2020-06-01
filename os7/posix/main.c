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

#include <semaphore.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

int sh_arr_size = 4;
int *sh_arr;
int shm_fd;
sem_t *from_sem;
sem_t *to_prep_sem;
sem_t *to_send_sem;
sem_t *free_sem;
sem_t *can_mod_sem;

void remove_sem(sem_t *sem, char *sem_name) {
    if (sem_close(sem) == -1)
        perror("sem_close");
    if (sem_unlink(sem_name) == -1)
        perror("sem_unlink");
}

void kill_all(int sig) {
    safe_print("\nRemoving ipcs...\n");
    // probably not async safe
    if (munmap(sh_arr, sh_arr_size) == -1)
        perror("munmap"); 
    if (shm_unlink(SHM_NAME) == -1)
        perror("shm_unlink");

    remove_sem(from_sem, FROM_SEM);
    remove_sem(to_prep_sem, TO_PREP_SEM);
    remove_sem(to_send_sem, TO_SEND_SEM);
    remove_sem(free_sem, FREE_SEM);
    remove_sem(can_mod_sem, CAN_MOD_SEM);

    safe_print("\nClosing...\n");    
    exit(EXIT_SUCCESS);
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


    // Shared Memory
    shm_fd = shm_open(SHM_NAME, CREATE_FLAGS, MODE);
    if (shm_fd == -1) 
        error(errno, "shm_open");
    if (ftruncate(shm_fd, sh_arr_size * sizeof(int)) == -1)
        error(errno, "ftruncate");
    // Shared Array
    sh_arr = mmap(NULL, sh_arr_size * sizeof(int), PROT, MAP_SHARED, shm_fd, 0);
    if (sh_arr == MAP_FAILED)
        error(errno, "mmap");
    // making semaphores
    from_sem = sem_open(FROM_SEM, CREATE_FLAGS, MODE, 0);
    if (from_sem == SEM_FAILED)
        error(errno, "from_sem sem_open");
    to_prep_sem = sem_open(TO_PREP_SEM, CREATE_FLAGS, MODE, 0);
    if (to_prep_sem == SEM_FAILED)
        error(errno, "to_prep_sem sem_open");
    to_send_sem = sem_open(TO_SEND_SEM, CREATE_FLAGS, MODE, 0);
    if (to_send_sem == SEM_FAILED)
        error(errno, "to_send_sem sem_open");
    free_sem = sem_open(FREE_SEM, CREATE_FLAGS, MODE, sh_arr_size);
    if (free_sem == SEM_FAILED)
        error(errno, "free_sem sem_open");
    can_mod_sem = sem_open(CAN_MOD_SEM, CREATE_FLAGS, MODE, 1);
    if (can_mod_sem == SEM_FAILED)
        error(errno, "can_mod_sem sem_open");

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