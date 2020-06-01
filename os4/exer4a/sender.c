#define _XOPEN_SOURCE 700
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include "shared.h"

int catcher_pid;
int sig_num;
char *mode;
int signals_catched;

void end_output(int sender_pid, int send_sig_num, union sigval value) {
    printf("Signals recieved in sender: %d\n", send_sig_num);
    printf("Signals sender sent:        %d\n\n", sig_num);
    if (strcmp(mode, SIGQ) == 0)
        printf("si_value sival_int:         %d\n\n", value.sival_int);
    _exit(EXIT_SUCCESS);
}

void handler(int sig, siginfo_t *info, void *ucontext) {
    char *output;
    if (sig == SIG)
        output = "Signal catched in sender\n";
    else if (sig == END_SIG) {
        end_output(info->si_pid, signals_catched, info->si_value);
    }
    else
        output = "Sth went wrong";
    signals_catched++;
    write(1, output, strlen(output));
}

void set_handler() {
    struct sigaction siga;
    siga.sa_flags = SA_SIGINFO;
    siga.sa_sigaction = handler;
    
    sigemptyset(&siga.sa_mask);
    sigaddset(&siga.sa_mask, SIG);
    sigaddset(&siga.sa_mask, END_SIG);
    
    // It says we should block every other signal but I don't think this is a good idea
    // sigfillset(&siga.sa_mask);
    // sigdelset(&siga.sa_mask, SIG);
    // sigdelset(&siga.sa_mask, END_SIG);
    sigaction(SIG, &siga, NULL);
    sigaction(END_SIG, &siga, NULL);
}


int main(int argc, char **argv) {
    catcher_pid = atoi(argv[1]);
    sig_num = atoi(argv[2]);
    mode = argv[3];
    signals_catched = 0;

    set_handler();

    printf("My pid:         %d\n", getpid());
    printf("Catcher pid:    %d\n", catcher_pid);
    printf("Sig num         %d\n", sig_num);
    printf("Mode            %s\n\n", mode);
    
    if (strcmp(mode, KILL) == 0) {
        for (int i = 0; i < sig_num; i++)
            kill(catcher_pid, SIG);
        kill(catcher_pid, END_SIG);
    } else if (strcmp(mode, SIGQ) == 0) {
        union sigval tmp; // we don't need it in sender
        for (int i = 0; i < sig_num; i++)
            sigqueue(catcher_pid, SIG, tmp);
        sigqueue(catcher_pid, END_SIG, tmp);
    }
    // while(true) to wait for signals from catcher
    while (true);

    return 0;
}