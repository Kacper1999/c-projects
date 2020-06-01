#pragma once
#define _XOPEN_SOURCE 700

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define COMM_TYPE SOCK_STREAM
#define MSG_SIZE 256
#define PORT_NUM 9002
#define SERV_PATH "./serv_f"
#define CLIENT_PATH "./client_f"

int init_socket(int addr_f, void *p);

struct sockaddr_in init_sockaddr_in(int addr_f, int port_num);
struct sockaddr_un init_sockaddr_un(int addr_f, char *path);

void send_msg(int socket, char msg[MSG_SIZE]);
void safe_print(char *msg);
void err_ex(char *msg);