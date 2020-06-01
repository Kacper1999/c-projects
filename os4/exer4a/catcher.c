#define _XOPEN_SOURCE 700
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>
#include <string.h>
#include <wait.h>

#include "shared.h"

int signals_catched;
int kill_sender_pid = -1;
int sigq_sender_pid = -1;

void error(char *message) {
    puts(message);
    exit(EXIT_FAILURE);
}

char* send_end(int sender_pid, int send_sig_num) {
    if (sender_pid == kill_sender_pid) {
        for (int i = 0; i < send_sig_num; i++) 
            kill(sender_pid, SIG);
        kill(sender_pid, END_SIG);
    } else if (sender_pid == sigq_sender_pid) {
        union sigval tmp;
        for (int i = 0; i < send_sig_num; i++) {
            tmp.sival_int = i;  // not sure if this is what was asked for
            sigqueue(sender_pid, SIG, tmp);
        }
        tmp.sival_int = send_sig_num;
        sigqueue(sender_pid, END_SIG, tmp);
    } else 
        return "Sth went wrong in sending from catcher\n";
    char *output = calloc(50, sizeof(char));
    // not async safe but how else
    sprintf(output, "Signals recieved in catcher:   %d\n\n", send_sig_num);
    return output;
}

void handler(int sig, siginfo_t *info, void *ucontext) {
    char *output;
    if (sig == SIG)
        output = "Signal catched in catcher\n";
    else if (sig == END_SIG) 
        output = send_end(info->si_pid, signals_catched);
    else
        output = "Sth went wrong\n";
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

void call_exec(char *sig_num, char *mode) {
    signals_catched = 0;
    printf("Calling sender mode: %s\n\n", mode);

    int arg_num = 4;
    char **arg = calloc(arg_num, sizeof(char*));
    char *ppid = calloc(20, sizeof(char));
    sprintf(ppid, "%d", getppid());

    arg[0] = SENDER_EXE;
    arg[1] = ppid;
    arg[2] = sig_num;
    arg[3] = mode;

    execvp(arg[0], arg);
    for (int i = 0; i < arg_num; i++)
        free(arg[i]);
    free(arg);
    exit(EXIT_SUCCESS);
}


int main(int argc, char **argv) {
    if (argc != 3)
        error("Wrong nubmer of arguments");
    char* sig_num = argv[1];
    char* mode = argv[2];
    pid_t my_pid = getpid();

    printf("Catcher PID: %d\n", my_pid);
    set_handler();

    pid_t pid;
    pid = fork();
    if (pid == 0)
        call_exec(sig_num, mode);
    else {
        if (strcmp(mode , KILL) == 0)
            kill_sender_pid = pid;
        else if (strcmp(mode, SIGQ) == 0)
            sigq_sender_pid = pid;
        else
            error("wrong mode");
    }
    wait(NULL);
    return 0;
}