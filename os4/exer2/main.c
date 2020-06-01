#define _XOPEN_SOURCE 700
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <dirent.h>
#include <stdbool.h>
#include <string.h>
#include <wait.h>

#define sig SIGUSR1

bool use_exec = false;

void sig_handler() {
    write(1, "sig handler\n", 12);
}

void call_exec(char* mode) {
    puts("using exec");
    int arg_num = 2;
    char **arg = calloc(arg_num, sizeof(char*));
    arg[0] = "./main_exec";
    arg[1] = mode;
    execvp("./main_exec", arg);
    for (int i = 0; i < arg_num; i++)
        free(arg[i]);
    free(arg);
}

void check(char* mode) {
    //  when using signal instead of sigaction using raise for the second time program crashes
    puts("parent");
    raise(sig);
    if (fork() == 0) {
        if (use_exec) {
            // ugly because check is used by ignore and handler but handler can never be used with exec
            call_exec(mode);
        } else {
            puts("child");
            raise(sig);
            exit(EXIT_SUCCESS);
        }
    }
    int status;
    wait(&status);
    if (WIFEXITED(status)) 
        puts("Child finished after handeling SIGUSR1");
    else
        puts("Child finished by default action of SIGUSR1");
}

void test_ignore() {
    signal(sig, SIG_IGN);
    check("ignore");
}

void handler_test() {
    struct sigaction siga;
    siga.sa_handler = sig_handler;
    sigemptyset(&siga.sa_mask);
    sigaction(sig, &siga, NULL);
    check("handler");
}

void check_pending() {
    sigset_t waiting_mask;
    sigemptyset(&waiting_mask);
    sigpending(&waiting_mask);
    if (sigismember(&waiting_mask, sig))
        puts("Sig in pending");
    else
        puts("Sig not in pending");
}

// didn't quite understand the question I hope this is enough
void mask_pend_check(char* mode) {
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, sig);
    sigprocmask(SIG_SETMASK, &mask, NULL);

    puts("parent");

    if (!strcmp(mode, "pending"))
        raise(sig);
    check_pending();
    if (fork() == 0) {
        if (use_exec) {
            call_exec(mode);
        } else {
            puts("Child before");
            check_pending();
            raise(sig);
            puts("Child after raise");
            check_pending();
            exit(EXIT_SUCCESS);
        }
    }
}

void mask_test() {
    mask_pend_check("mask");
}

void pending_test() {    
    mask_pend_check("pending");
}

int main(int argc, char** argv) {
    if (argc == 3)
        use_exec = true;
    char* mode = argv[1];
    if (!strcmp(mode, "ignore"))
        test_ignore();
    else if (!strcmp(mode, "handler"))
        handler_test();
    else if (!strcmp(mode, "mask"))
        mask_test();
    else if (!strcmp(mode, "pending"))
        pending_test();
    else
        puts("wrong input");
    return 0;
}