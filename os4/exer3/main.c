#define _XOPEN_SOURCE 700
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>

#define my_sig SIGTSTP

// no idea what "scenarios" should I come up with
// and what "well chosen" signals are
void handler(int sig, siginfo_t *info, void *ucontext) {
    printf("\nUser time consumed      %ld\n", info->si_utime);
    printf("Exit value or signal    %d\n", info->si_status);
    printf("sival_int               %d\n", info->si_value.sival_int);
}

int main() {
    struct sigaction siga;
    siga.sa_flags = SA_SIGINFO;
    siga.sa_sigaction = handler;
    sigemptyset(&siga.sa_mask);
    sigaction(my_sig, &siga, NULL);

    while (true) {
        sleep(1);
    }

    return 0;
}