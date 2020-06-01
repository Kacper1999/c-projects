#define _XOPEN_SOURCE 700
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>
#include <wait.h>

int main() {
    union sigval tmp;
    tmp.sival_int = 100;
    printf("%d\n", tmp.sival_int);
    return 0;
}