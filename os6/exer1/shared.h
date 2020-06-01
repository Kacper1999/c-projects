#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>


#define SERV_CLIENT_CHAT_INIT 1
#define SERV_CLIENT_TERMINATE 2
#define SERV_CLIENT_REGISTRED 3
#define CLIENT_CLIENT_MSG 4
#define CLIENT_CLIENT_DICONNECT 5

#define SERV_MAX_CLIENTS_CAPACITY 32
#define SERV_MESSAGE_TYPE_PRIORITY (-6)
#define MAX_MSG_LENGTH 128

#define KEY_GENERATOR_PATH (getenv("HOME"))
#define PROJECT_IDENTIFIER (112358)
const int ID_NUMS[SERV_MAX_CLIENTS_CAPACITY];

#define CLIENT_SERV_STOP 1
#define CLIENT_SERV_DISCONNECT 2
#define CLIENT_SERV_LIST 3
#define CLIENT_SERV_CONNECT 4
#define CLIENT_SERV_INIT 5

#define UNIQUE_KEY (ftok(KEY_GENERATOR_PATH, getpid()))
#define SERV_KEY (ftok(KEY_GENERATOR_PATH, PROJECT_IDENTIFIER))

#define DELETE_QUEUE(id) (msgctl(id, IPC_RMID, NULL))
#define CREATE_QUEUE(key) (msgget(key, 0666 | IPC_CREAT | IPC_EXCL))
#define GET_QUEUE(key) (msgget(key, 0666))

#define RECEIVE_MESSAGE_NO_WAIT(id, msg_ptr, type)                          \
  (msgrcv(id, msg_ptr, sizeof(*msg_ptr), type, IPC_NOWAIT))

#define RECEIVE_MESSAGE(id, msg_ptr, type)                                  \
  (msgrcv(id, msg_ptr, sizeof(*msg_ptr), type, 0))

#define SEND_MESSAGE(id, msg_ptr)                                           \
  (msgsnd(id, msg_ptr, sizeof(*msg_ptr), 0))

#define MESSAGE_SIG SIGRTMIN

struct client {
  key_t key;
  int   client_id;
  int   client_pid;
  int   q_id;
  int   available;
} typedef client;

struct client_client_msg {
  long type;
  char msg[MAX_MSG_LENGTH];
} typedef client_client_msg;


struct client_serv_msg {
  long  type;
  int   client_id;
  key_t client_key;
  int   client_pid;
  int   my_chat_id;
} typedef client_serv_msg;


struct serv_client_msg {
  long  type;
  int   client_id;
  key_t my_chat_key;
  int   my_chat_pid;
} typedef serv_client_msg;

int  q_empty(int q_id);

int  str_equals(char* str1, char* str2);
void error();