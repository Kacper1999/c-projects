#include "shared.h"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>


struct sockaddr_in init_sockaddr_in(int addr_f, int port_num) {
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = addr_f;
    serv_addr.sin_port = htons(port_num);
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    return serv_addr;
}

struct sockaddr_un init_sockaddr_un(int addr_f, char *path) {
    struct sockaddr_un serv_addr;
    serv_addr.sun_family = addr_f;
    strcpy(serv_addr.sun_path, path);
    return serv_addr;
}


int init_net_socket(int port_num) {
    int addr_f = AF_INET;
    int net_socket = socket(addr_f, COMM_TYPE, 0);
    if (net_socket == -1) 
        err_ex("net socket()");

    struct sockaddr_in serv_addr = init_sockaddr_in(addr_f, port_num);

    if (bind(net_socket, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
        err_ex("net bind");
    return net_socket;
}

int init_local_socket(char *path) {
    int addr_f = AF_UNIX;
    int local_socket = socket(addr_f, COMM_TYPE, 0);
    if (local_socket == -1)
        err_ex("local socket()");

    struct sockaddr_un serv_addr = init_sockaddr_un(addr_f, path);
    
    if (bind(local_socket, (struct sockaddr*)&serv_addr, sizeof(serv_addr)))
        err_ex("local bind");
    return local_socket;
}

int init_socket(int addr_f, void *p) {
    if (addr_f == AF_INET)
        return init_net_socket((long)p);
    return init_local_socket((char *)p);
}

void send_msg(int socket, char msg[MSG_SIZE]) {
    send(socket, msg, MSG_SIZE, 0);
}

void safe_print(char *msg) {
    write(1, msg, strlen(msg));
}

void err_ex(char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
} 