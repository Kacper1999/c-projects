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

int main(int argc, char** argv){
    if (argc != 2) 
        error("Wrong number of arguments");
    char *file_path = argv[1];
    FILE *f = fopen(file_path, "r");
    if (f == NULL) 
        error("Coudn't open file");

    FILE *sorted = popen("sort", "w");
    char buf[MAX_LEN];

    while(fgets(buf, MAX_LEN, f) != NULL)
        fputs(buf, sorted);
    pclose(sorted);
    return 0;
}