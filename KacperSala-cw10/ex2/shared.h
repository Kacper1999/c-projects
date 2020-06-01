#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sys/epoll.h>

#include <poll.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/sockios.h>
#include <errno.h>

#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <pthread.h>

#define PING_INTERVAL 10
#define PING_WAIT 5
#define MAX_CLIENTS 13
#define MSG_SIZE 30
#define NAME_LEN 8

enum msg_type{
	CONNECT,
	CONNECT_FAILED,
	PING,
	WAIT,
	GAME_FOUND,
	MOVE,
	GAME_FINISHED,
	DISCONNECT,
	EMPTY
}; typedef enum msg_type msg_type;
enum connection_type{
	LOCAL,
	INET
}; typedef enum connection_type connection_type;



struct game{
	char board[9];
	char turn;		
	char winner;	
}; typedef struct game game;

struct client{
	int fd;	
	struct sockaddr* addr;
	char* name;	
	int active;
	int enemy_i;
	game* game;
	char symbol;
}; typedef struct client client;

struct message{
	msg_type msg_type;
	game game;
	char name[NAME_LEN];
}; typedef struct message message;

message rcv_msg_block(int fd);
message rcv_msg_addr(int fd, struct sockaddr* addr, socklen_t len);
message rcv_msg(int fd);

void new_board(game* game);
void error(char* msg);
void send_msg(int fd, msg_type type, game* game, char* nick);
void send_msg_addr(int fd, struct sockaddr* addr, msg_type type, game* game, char* nick);
