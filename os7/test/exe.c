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
#include <sys/msg.h> 
#include <time.h>

#include <semaphore.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

int sh_arr_size = 4;
int shm_fd;
char *shm_name = "shm_name";

int main(int argc, char **argv) {
    printf("hi from fork\n");

    shm_fd = shm_open(shm_name, O_RDWR, 0);
    if (shm_fd == -1)
        perror("exe shm_open");
    int *sh_arr = mmap(NULL, sh_arr_size * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (sh_arr == (int *)-1)
        perror("exe mmap");

    struct stat buf;
    printf("init shm_fd: %d\n", shm_fd);
    if (stat(shm_name, &buf) == -1)
        perror("init stat");
    printf("buf.size: %ld\n", buf.st_size);
    
    printf("exe 1 sh_arr[0]: %d\n", sh_arr[0]);
    sh_arr[0] *= -1;
    printf("exe 2 sh_arr[0]: %d\n", sh_arr[0]);
    sleep(2);
    sh_arr[0]++;
    printf("exe 3 sh_arr[0]: %d\n", sh_arr[0]);
    return 0;
}