#define _XOPEN_SOURCE 700
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <dirent.h>
#include <stdbool.h>
#include <string.h>

#define sig SIGUSR1

void check() {
    raise(sig);
    exit(EXIT_SUCCESS);
}

void test_ignore() {
    check();
}

void handler_test() {
    check();
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
void mask_test() {
    puts("Child before raise");
    check_pending();
    puts("Child after raise");
    raise(sig);
    check_pending();
    exit(EXIT_SUCCESS);    
}

void pending_test() {
    puts("Child before raise");
    check_pending();
    puts("Child after raise");
    raise(sig);
    check_pending();
    exit(EXIT_SUCCESS);     
}

int main(int argc, char** argv) {
    char* mode = argv[1];
    if (!strcmp(mode, "ignore")) 
        test_ignore();
    else if (!strcmp(mode, "mask"))
        mask_test();
    else if (!strcmp(mode, "pending"))
        pending_test();
    else if (!strcmp(mode, "handler"))
        handler_test();
    else
        puts("sth wrong");
    return 0;
}