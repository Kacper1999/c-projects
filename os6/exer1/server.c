#include "shared.h"
#include <signal.h>

client* clients[SERV_MAX_CLIENTS_CAPACITY];

int serv_q_id = -1;
int clients_num = 0;
int w8_4_clients = 0;
int curr = 0;
void exit_serv() {
    printf("Server: shuting down\n");
    exit(EXIT_SUCCESS);
}


void handle_connect(client_serv_msg* msg) {
    int id1 = msg->client_id;
    int id2 = msg->my_chat_id;
    if (!clients[id2] || !clients[id2]->available) {
        printf("Server: client is not avaiable\n");
        return;
    }
    clients[id1]->available = 0;
    clients[id2]->available = 0;

    serv_client_msg* msg1 = malloc(sizeof(serv_client_msg));
    serv_client_msg* msg2 = malloc(sizeof(serv_client_msg));

    msg1->type = SERV_CLIENT_CHAT_INIT;
    msg2->type = SERV_CLIENT_CHAT_INIT;
    msg1->my_chat_key = clients[id2]->key;
    msg1->my_chat_pid = clients[id2]->client_pid;
    msg2->my_chat_key = clients[id1]->key;
    msg2->my_chat_key = clients[id1]->client_pid;
    msg1->client_id = id2;
    msg2->client_id = id1;

    SEND_MESSAGE(clients[id1]->q_id, msg1);
    SEND_MESSAGE(clients[id2]->q_id, msg2);
    kill(clients[id1]->client_pid, MESSAGE_SIG);
    kill(clients[id2]->client_pid, MESSAGE_SIG);
    printf("Server: started chat, %d <=> %d\n", id1, id2);
}

void handle_init(client_serv_msg* msg) {
    int ptr = -1;
    for (int i = 0; i < SERV_MAX_CLIENTS_CAPACITY; i++) {
        ptr = (curr + i) % SERV_MAX_CLIENTS_CAPACITY;
        if (!clients[ptr]) 
            break;
    }

    if (ptr == -1) {
        printf("Server: cannot add client reached max capcity\n");
    } else {
        client* client = malloc(sizeof(client));
        client->available = 1;
        client->key = msg->client_key;
        client->client_id = ptr;
        client->client_pid = msg->client_pid;
        client->q_id = GET_QUEUE(msg->client_key);

        clients[ptr] = client;
        serv_client_msg* scMsg = malloc(sizeof(serv_client_msg));
        scMsg->type = SERV_CLIENT_REGISTRED;
        scMsg->client_id = ptr;
        SEND_MESSAGE(client->q_id, scMsg);
        clients_num++;
        printf("Server: client id: %d, key: %d registered\n", client->client_id, client->key);
        free(scMsg);
    }
}

void handle_disconnect(client_serv_msg* msg) {
  printf("Server: client with id: %d disconnected\n", msg->client_id);
  clients[msg->client_id]->available = 1;
}


void handle_stop(client_serv_msg* msg) {
    clients[msg->client_id] = NULL;
    free(clients[msg->client_id]);
    clients_num -= 1;
    printf("Server: got STOP from client with id: %d\n", msg->client_id);
    if (w8_4_clients && clients_num <= 0) 
        exit_serv();
}


void handle_list(client_serv_msg* msg) {
    printf("Server: client with id: %d requested list", msg->client_id);
    for (int i = 0; i < SERV_MAX_CLIENTS_CAPACITY; i++) {
        if (i == msg->client_id) 
            printf("\tclient:> id - %d, key - %d (ME)\n", clients[i]->client_id, clients[i]->key);
        else if (clients[i] && clients[i]->available) 
            printf("\tclient:> id - %d, key - %d\n", clients[i]->client_id, clients[i]->key);
    }
}


void handle_msg() {
    client_serv_msg* msg = malloc(sizeof(client_serv_msg*));
    RECEIVE_MESSAGE(serv_q_id, msg, SERV_MESSAGE_TYPE_PRIORITY);
    int type = msg->type;
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
    free(msg);
}


void handle_exit_sig(int sig) {
    serv_client_msg* msg = malloc(sizeof(serv_client_msg));
    msg->type = SERV_CLIENT_TERMINATE;
    if (clients_num <= 0)
        exit_serv();
    w8_4_clients = 1;
    for (int i = 0; i < SERV_MAX_CLIENTS_CAPACITY; i++) {
        if (clients[i]) {
            SEND_MESSAGE(clients[i]->q_id, msg);
            kill(clients[i]->client_pid, MESSAGE_SIG);
        }
    }
    printf("Server: waiting till every client exits\n");
}
void at_exit() { DELETE_QUEUE(serv_q_id); }

int main(int argc, char* arrgv[]) {
    serv_q_id = CREATE_QUEUE(SERV_KEY);

    signal(SIGINT, handle_exit_sig);
    atexit(at_exit);

    printf("Server running...\n");
    while (1) 
        handle_msg();
    return 0;
}