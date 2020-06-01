#define _XOPEN_SOURCE 700
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
#include <string.h>
#include <time.h>
#include <sys/msg.h> 

#include <semaphore.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

int child_pid;
int *sh_arr;
int sh_arr_size = 4;
int shm_fd;
char *shm_name = "/shm_name";
sem_t *sem;
char *sem_name = "/sem_name";


void han(int sig) {
    if (munmap(sh_arr, sh_arr_size) == -1)
        perror("munmap"); 
    if (shm_unlink(shm_name) == -1)
        perror("shm_unlink");

    if (sem_close(sem) == -1)
        perror("sem remove");
    exit(EXIT_SUCCESS);
}


int main() {
    // handle ^C
    struct sigaction hk;
    hk.sa_handler = han;
    sigaction(SIGINT, &hk, NULL);

    // some const
    int create_flags = O_CREAT | O_RDWR; // | O_EXCL
    int mode = 0777;

    // semaphore
    sem = sem_open(sem_name, create_flags, mode, 1);
    if (sem == SEM_FAILED)
        perror("sem_open");

    // shared mem
    shm_fd = shm_open(shm_name, create_flags, mode);
    if (shm_fd == -1)
        perror("shm_open");
    // shared arr
    if (ftruncate(shm_fd, sh_arr_size * sizeof(int)) == -1)
        perror("ftruncate");
    sh_arr = mmap(NULL, sh_arr_size * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (sh_arr == (int *)-1)
        perror("mmap");
    sh_arr[0] = 1;

    if (fork() == 0)
        execl("./exe", "./exe", NULL);
    

    sleep(1);
    printf("sh_arr[0]: %d\n", sh_arr[0]);
    sh_arr[0] *= -1;
    printf("sh_arr[0]: %d\n", sh_arr[0]);

    sem_t *all_sems[] = {sem, sem};
    int sval;
    sem_getvalue(all_sems[1], &sval);
    printf("all_sems[1]->val: %d\n", sval);

    while(true);
    // remove ipcs
    if (munmap(sh_arr, sh_arr_size) == -1)
        perror("munmap"); 
    if (shm_unlink(shm_name) == -1)
        perror("shm_unlink");

    if (sem_close(sem) == -1)
        perror("sem remove");
    return 0;
}