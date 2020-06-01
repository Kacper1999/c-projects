#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <mqueue.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <linux/limits.h>
#define _GNU_SOURCE

#define SERV_CLIENT_CHAT_INIT 1
#define SERV_CLIENT_TERMINATE 2
#define SERV_CLIENT_REGISTRED 3
#define CLIENT_CLIENT_MSG 4
#define CLIENT_CLIENT_DICONNECT 5

#define SERV_MAX_CLIENTS_CAPACITY 32
#define MAX_MSG_LENGTH 256

#define CLIENT_SERV_STOP 5
#define CLIENT_SERV_DISCONNECT 4
#define CLIENT_SERV_LIST 3
#define CLIENT_SERV_CONNECT 2
#define CLIENT_SERV_INIT 1

#define Q_GENERATOR_PREFIX ("/queues-")
#define Q_SERV_NAME (concat(Q_GENERATOR_PREFIX, "server"))
#define Q_CLIENT_RANDOM_NAME                                               \
  (concat(Q_GENERATOR_PREFIX, concat("client-", r_str(12))))

#define CLOSE_Q(descr) (mq_close(descr))
#define CREATE_Q(name) (get_q(name))
#define DELETE_Q(name) (mq_unlink(name))
#define GET_Q(name) (mq_open(name, O_WRONLY))
#define SEND_MSG(desc, msg_ptr, type)                                   \
  (mq_send(desc, msg_ptr, strlen(msg_ptr), type))
#define RECEIVE_MSG(desc, msg_ptr, typePointer)                         \
  (mq_receive(desc, msg_ptr, MAX_MSG_LENGTH, typePointer))
#define REGISTER_NOTIFICATION(desc, sigevent) (mq_notify(desc, sigevent))

struct client {
    char* name;
    int   id;
    int   available;
    int   q_desc;
} typedef client;

void  error();
int   str_equal(char* str1, char* str2);

char* r_str(int length);
char* concat(const char* s1, const char* s2);
char* get_cwd();
int   get_q(char* name);