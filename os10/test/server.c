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


void kill_all(int sig) {
    safe_print("\nDeleting serv file...\n");
    unlink(SERV_PATH);
    exit(EXIT_SUCCESS);
}

int main() {
    // handle ^C
    struct sigaction new_handle;
    new_handle.sa_handler = kill_all;
    sigaction(SIGINT, &new_handle, NULL);

    int net_socket = init_socket(AF_INET, (void *)PORT_NUM);
    int local_socket = init_socket(AF_UNIX, (void *)SERV_PATH);

    listen(local_socket, 5);

    int client_socket = accept(local_socket, NULL, NULL);
    send_msg(client_socket, "hi");


    close(local_socket);
    close(net_socket);
    unlink(SERV_PATH);
    return 0;
}