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
    if (argc != 3)
        error("Wrong number of arguments");

    FILE* in = fopen(argv[1], "r");
    FILE* out = fopen(argv[2], "w");
    int tm;
    char buf[MAX_LEN+1];

    while (fscanf(in, "#%d#%s\n", &tm, buf) != EOF) {
        fprintf(out, "%s\n", buf);
        printf("%s\n", buf);
    }
    fclose(in);
    fclose(out);
    return 0;
}