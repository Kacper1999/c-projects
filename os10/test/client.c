#include "shared.h"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int my_socket;

void kill_all(int sig) {
    safe_print("\nDeleting serv file...\n");
    unlink(CLIENT_PATH);
    close(my_socket);
    exit(EXIT_SUCCESS);
}

int main() {
    // handle ^C
    struct sigaction new_handle;
    new_handle.sa_handler = kill_all;
    sigaction(SIGINT, &new_handle, NULL);

    int addr_f = AF_UNIX;
    my_socket = init_socket(addr_f, (void *)CLIENT_PATH);

    struct sockaddr_un serv_addr = init_sockaddr_un(addr_f, SERV_PATH);

    connect(my_socket, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    char msg[MSG_SIZE];
    recv(my_socket, msg, MSG_SIZE, 0);

    printf("we go: %s\n", msg);

    close(my_socket);
    unlink(CLIENT_PATH);
    return 0;
}