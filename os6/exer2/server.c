#include "shared.h"
#include <signal.h>

int curr = 0;
int w8_4_clients = 0;
int clients_num = 0;
int serv_q_desc = -1;
client* clients[SERV_MAX_CLIENTS_CAPACITY];

void exit_serv() {
    printf("Server: shuting down\n");
    CLOSE_Q(serv_q_desc);
    DELETE_Q(Q_SERV_NAME);
    exit(EXIT_SUCCESS);
}

void handle_exit_sig(int signal) {
    char msg[MAX_MSG_LENGTH];
    sprintf(msg, "%d", SERV_CLIENT_TERMINATE);
    if (clients_num <= 0)
        exit_serv();
    w8_4_clients = 1;
    for (int i = 0; i < SERV_MAX_CLIENTS_CAPACITY; i++) 
        if (clients[i]) 
            SEND_MSG(clients[i]->q_desc, msg, SERV_CLIENT_TERMINATE);
    printf("Server: waiting for all clients to exit.\n");
}

void handle_stop(char* msg) {
    int type, id;
    sscanf(msg, "%d %d", &type, &id);
    clients[id] = NULL;
    free(clients[id]);
    clients_num -= 1;
    printf("Server: got STOP from %d\n", id);
    if (w8_4_clients && clients_num <= 0) 
        exit_serv();
}
void handle_disconnect(char* msg) {
    int type, id;
    sscanf(msg, "%d %d", &type, &id);
    clients[id]->available = 1;
    printf("Server: client with id %d left chat\n", id);
}

void handle_list(char* msg) {
    int type, id;
    sscanf(msg, "%d %d", &type, &id);
    printf("Server: client with id: %d\n requested LIST", id);
    for (int i = 0; i < SERV_MAX_CLIENTS_CAPACITY; i++) {
        if (i == id) 
            printf("\tClient:> id - %d, path - %s (ME)\n", clients[i]->id, clients[i]->name);
        else if (clients[i] && clients[i]->available)
            printf("\tClient:> id - %d, path - %s\n", clients[i]->id, clients[i]->name);
    }
}

void handle_connect(char* msg) {
    int type, id, my_chatId;
    sscanf(msg, "%d %d %d", &type, &id, &my_chatId);

    int id1 = id;
    int id2 = my_chatId;

    if (id2 < 0 || id2 >= SERV_MAX_CLIENTS_CAPACITY || !clients[id2] ||
        !clients[id2]->available || id1 == id2) {
        printf("Server: requested client is not avaiable\n");
        return;
    }
    char msg1[MAX_MSG_LENGTH];
    char msg2[MAX_MSG_LENGTH];

    sprintf(msg1, "%d %d %s", SERV_CLIENT_CHAT_INIT, id2, clients[id2]->name);
    sprintf(msg2, "%d %d %s", SERV_CLIENT_CHAT_INIT, id1, clients[id1]->name);

    clients[id1]->available = 0;
    clients[id2]->available = 0;

    SEND_MSG(clients[id1]->q_desc, msg1, SERV_CLIENT_CHAT_INIT);
    SEND_MSG(clients[id2]->q_desc, msg2, SERV_CLIENT_CHAT_INIT);

    printf("Server: initialized chat, %d <=> %d\n", id1, id2);
}

void handle_init(char* msg) {
    int   type;
    char* name = malloc(MAX_MSG_LENGTH * sizeof(char));
    sscanf(msg, "%d %s", &type, name);

    int ptr = -1;
    for (int i = 0; i < SERV_MAX_CLIENTS_CAPACITY; i++) {
        ptr = (curr + i) % SERV_MAX_CLIENTS_CAPACITY;
        if (!clients[ptr])
            break;
    }
    if (ptr == -1) {
        printf("Server: cannot add another client, reached max capcity\n");
    } else {
        client* client = malloc(sizeof(client));
        client->q_desc = GET_Q(name);
        client->id = ptr;
        client->available = 1;
        client->name = name;
        clients[ptr] = client;
        char scMsg[MAX_MSG_LENGTH];
        sprintf(scMsg, "%d %d", SERV_CLIENT_REGISTRED, ptr);
        SEND_MSG(client->q_desc, scMsg, SERV_CLIENT_REGISTRED);
        clients_num += 1;
        printf("Server:  client - id: %d, path: %s registered\n", client->id, client->name);
  }
}

void handle_msg() {
    char* msg = malloc(sizeof(char) * MAX_MSG_LENGTH);
    unsigned int type;
    RECEIVE_MSG(serv_q_desc, msg, &type);
    if (type == CLIENT_SERV_STOP)
        handle_stop(msg);
    else if (type == CLIENT_SERV_DISCONNECT)
        handle_disconnect(msg);
    else if (type == CLIENT_SERV_LIST)
        handle_list(msg);
    else if (type == CLIENT_SERV_CONNECT)
        handle_connect(msg);
    else if (type == CLIENT_SERV_INIT) 
        handle_init(msg);
    else 
        printf("Unknown type\n");
    free(msg);
}

int main(int argc, char* arrgv[]) {
    serv_q_desc = CREATE_Q(Q_SERV_NAME);
    signal(SIGINT, handle_exit_sig);
    printf("Server running...\n");
    while (1) 
        handle_msg();
  return 0;
}