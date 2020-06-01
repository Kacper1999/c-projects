#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

#include <semaphore.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

#define MAX_LEN 10000
int main() {
	struct region {        /* Defines "structure" of shared memory */
		int len;
		char buf[MAX_LEN];
	};
	struct region *rptr;
	int fd;


	fd = shm_open("myregion", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
	if (fd == -1)
		perror("shm_open");


	if (ftruncate(fd, sizeof(struct region)) == -1)
		perror("ftruncate");

	rptr = mmap(NULL, sizeof(struct region),
		PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (rptr == MAP_FAILED)
		perror("mmap");
	
	rptr->len = 5;
	printf("main len: %d\n", rptr->len);
	if (fork() == 0) {
		int ffd = shm_open("myregion", O_RDWR, S_IRUSR | S_IWUSR);
		if (ffd == -1)
			perror("fork shm_open");
		struct region *fork_rptr = mmap(NULL, sizeof(struct region), PROT_READ | PROT_WRITE, MAP_SHARED, ffd, 0);
		printf("fork len: %d\n", fork_rptr->len);
	}
}