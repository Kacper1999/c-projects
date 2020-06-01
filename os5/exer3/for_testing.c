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

int main(int argc, char** argv) {
    if (argc != 4)
        error("Wrong number of arguments");
    int file = open(argv[1], O_WRONLY);
    FILE* in_file = fopen(argv[2], "r");
    int len = atoi(argv[3]);

    char buf[MAX_LEN+1];
    close(file);
    fclose(in_file);
    return 0;
}