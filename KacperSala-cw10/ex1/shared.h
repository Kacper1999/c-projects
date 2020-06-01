#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sys/epoll.h>
#include <errno.h>
#include <poll.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/sockios.h>

#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <pthread.h>

#define MAX_CLIENTS 13
#define MSG_SIZE 20
#define NAME_LEN 8

#define PING_INTERVAL 10
#define PING_WAIT 5

enum connection_type{
	LOCAL,
	INET
}; typedef enum connection_type connection_type;

enum message_type{
	CONNECT,
	CONNECT_FAILED,
	PING,
	WAIT,
	GAME_FOUND,
	MOVE,
	GAME_FINISHED,
	DISCONNECT,
	EMPTY
}; typedef enum message_type message_type;

struct game{
	char board[9];
	char turn;
	char winner;
}; typedef struct game game;

struct client{
	int fd;
	char* name;	
	game* game;
	int active;
	int opponent_idx;
	char symbol; 
}; typedef struct client client;

struct message{
	message_type message_type;
	game game;
	char name[NAME_LEN];
}; typedef struct message message;


void error(char* message);
void new_board(game* game);
void send_msg(int fd, message_type type, game* game, char* name);

message receive_message(int fd, int nonblocking);