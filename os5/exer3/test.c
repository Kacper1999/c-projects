#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <time.h>
#include <sys/times.h>
#include <signal.h>

#define MAX_LEN 100
void error(char *message) {
    printf("%s\n", message);
    exit(EXIT_FAILURE);
}

// really hard coded test
int main(int argc, char** argv) {
    if (argc != 9)  
        error("Wrong number of arguments");
    if (mkfifo(argv[1], 0666))
        error("mkfifo failed");

    pid_t pids[6];
    if ((pids[0] = fork()) == 0)
        execl("./consumer", "./consumer", argv[1], argv[8], NULL);
    for (int i = 0; i < 5; i++)
        if((pids[1 + i] = fork()) == 0)  // maybe I should name those argv[i] but I'm lazy
            execl("./producer", "./producer", argv[1], argv[2 + i], argv[7], NULL);
    for (int i = 0; i < 6; i++)
        waitpid(pids[i], NULL, 0);

    return 0;
}