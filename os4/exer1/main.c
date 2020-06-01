#define _XOPEN_SOURCE 700
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <dirent.h>
#include <stdbool.h>
#include <string.h>

int print = true;

void int_handle() {
    _exit(0);
}

void tstp_handle() {
    char *tmp ="Waiting for CTRL+Z - continuation or CTR+C - close\n";
    write(1, tmp, strlen(tmp));
    print = !print;
}

void print_dir() {
    DIR *d;
    struct dirent *dir;
    d = opendir(".");
    while ((dir = readdir(d)) != NULL) 
        printf("%s\n", dir->d_name);
    closedir(d);
}

int main() {
    struct sigaction siga;
    siga.sa_handler = tstp_handle;
    sigemptyset(&siga.sa_mask);

    sigaction(SIGTSTP, &siga, NULL);
    signal(SIGINT, int_handle);
    while (true) {
        if (print) 
            print_dir();
    }
    return 0;
}