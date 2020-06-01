#include <signal.h>
#include "shared.h"

key_t key;
int my_chat_pid = -1;
int my_chat_q_id = -1;
int client_q_id;
int client_id;
int serv_q_id;

void client_exit() {
    printf("Client: exit\n");
    exit(EXIT_SUCCESS);
}

void register_me() {
    client_serv_msg* cs_msg = malloc(sizeof(client_serv_msg));
    cs_msg->client_key = key;
    cs_msg->type = CLIENT_SERV_INIT;
    cs_msg->client_pid = getpid();
    SEND_MESSAGE(serv_q_id, cs_msg);
    serv_client_msg* sc_msg = malloc(sizeof(serv_client_msg));
    RECEIVE_MESSAGE(client_q_id, sc_msg, SERV_CLIENT_REGISTRED);
    client_id = sc_msg->client_id;
    printf("Client: I'm registered with id: %d, key: %d\n", client_id, key);
    free(cs_msg);
    free(sc_msg);
}

void send_stop() {
    printf("Client: sending STOP\n");
    client_serv_msg* msg = malloc(sizeof(client_serv_msg));
    msg->client_id = client_id;
    msg->type = CLIENT_SERV_STOP;

    SEND_MESSAGE(serv_q_id, msg);
    free(msg);
    client_exit();
}
void send_disconnect() {
    printf("Client: sending DISCONNECT\n");
    client_serv_msg* msg = malloc(sizeof(client_serv_msg));
    msg->client_id = client_id;
    msg->type = CLIENT_SERV_DISCONNECT;
    SEND_MESSAGE(serv_q_id, msg);
    free(msg);
    if (my_chat_q_id != -1) {
        client_client_msg* cc_msg = malloc(sizeof(client_client_msg));
        cc_msg->type = CLIENT_CLIENT_DICONNECT;

        SEND_MESSAGE(my_chat_q_id, cc_msg);
        kill(my_chat_pid, MESSAGE_SIG);
        my_chat_q_id = -1;
        my_chat_pid = -1;
        free(cc_msg);
    }
}

void send_connect(int my_chat_id) {
    printf("client: seending CONNECT to: %d\n", my_chat_id);
    client_serv_msg* msg = malloc(sizeof(client_serv_msg));
    msg->client_id = client_id;
    msg->my_chat_id = my_chat_id;
    msg->type = CLIENT_SERV_CONNECT;
    SEND_MESSAGE(serv_q_id, msg);
    free(msg);
}

void send_msg(char* message) {
    client_client_msg* msg = malloc(sizeof(client_client_msg));
    msg->type = CLIENT_CLIENT_MSG;
    strcpy(msg->msg, message);
    printf("Me: %s\n", msg->msg);
    SEND_MESSAGE(my_chat_q_id, msg);
    kill(my_chat_pid, MESSAGE_SIG);
}
void send_list() {
    printf("Client: sending LIST..\n");
    client_serv_msg* msg = malloc(sizeof(client_serv_msg));
    msg->type = CLIENT_SERV_LIST;
    msg->client_id = client_id;
    SEND_MESSAGE(serv_q_id, msg);
    free(msg);
}



void handle_exit_sig(int sig) { send_stop(); }

void handle_disconnect(client_client_msg* msg) {
    printf("client: got DISCONNECT\n");
    my_chat_q_id = -1;
    my_chat_pid = -1;
    send_disconnect();
}
void handle_terminate() {
    printf("client: got TERMINATE\n");
}

void handle_msg(client_client_msg* msg) {
    printf("Chatee says:\t%s\n", msg->msg);
}

void handle_init(serv_client_msg* msg) {
    printf("client: chat with %d..\n", msg->client_id);
    my_chat_q_id = GET_QUEUE(msg->my_chat_key);
    my_chat_pid = msg->my_chat_pid;
}


void handle_msg_sig(int sig) {
    while (!q_empty(client_q_id)) {
        serv_client_msg* sc_msg = malloc(sizeof(serv_client_msg));
        client_client_msg* cc_msg = malloc(sizeof(client_client_msg));
        if (RECEIVE_MESSAGE_NO_WAIT(client_q_id, sc_msg, SERV_CLIENT_CHAT_INIT) != -1) 
            handle_init(sc_msg);
        if (RECEIVE_MESSAGE_NO_WAIT(client_q_id, sc_msg, SERV_CLIENT_TERMINATE) != -1)
            handle_terminate();
        if (RECEIVE_MESSAGE_NO_WAIT(client_q_id, cc_msg, CLIENT_CLIENT_MSG) != -1)
            handle_msg(cc_msg);
        if (RECEIVE_MESSAGE_NO_WAIT(client_q_id, cc_msg, CLIENT_CLIENT_DICONNECT) != -1)
            handle_disconnect(cc_msg);
        free(sc_msg);
        free(cc_msg);
    }
}
void at_exit() { DELETE_QUEUE(client_q_id); }

int main(int argc, char** argv) {
    key = UNIQUE_KEY;
    client_q_id = CREATE_QUEUE(key);
    serv_q_id = GET_QUEUE(SERV_KEY);
    signal(SIGINT, handle_exit_sig);
    signal(MESSAGE_SIG, handle_msg_sig);
    atexit(at_exit);

    register_me();
    char buffer[64];
    char message[MAX_MSG_LENGTH];
    while (1) {
        while (!q_empty(client_q_id)) {
            serv_client_msg* sc_msg = malloc(sizeof(serv_client_msg));
            client_client_msg* cc_msg = malloc(sizeof(client_client_msg));
            if (RECEIVE_MESSAGE_NO_WAIT(client_q_id, sc_msg, SERV_CLIENT_CHAT_INIT) != -1) 
                handle_init(sc_msg);
            if (RECEIVE_MESSAGE_NO_WAIT(client_q_id, sc_msg, SERV_CLIENT_TERMINATE) != -1)
                handle_terminate();
            if (RECEIVE_MESSAGE_NO_WAIT(client_q_id, cc_msg, CLIENT_CLIENT_MSG) != -1)
                handle_msg(cc_msg);
            if (RECEIVE_MESSAGE_NO_WAIT(client_q_id, cc_msg, CLIENT_CLIENT_DICONNECT) != -1)
                handle_disconnect(cc_msg);
            free(sc_msg);
            free(cc_msg);
        }
        scanf("%s", buffer);
        if (str_equals(buffer, "STOP"))
            send_stop();
        else if (str_equals(buffer, "LIST"))
            send_list();
        else if (str_equals(buffer, "DISCONNECT"))
            send_disconnect();
        else if (str_equals(buffer, "CONNECT")) {
            int my_chat_id;
            scanf("%d", &my_chat_id);
            send_connect(my_chat_id);
        } else if (str_equals(buffer, "SEND")) {
            scanf("%s", message);
            send_msg(message);
        }
  }
  return 0;
}
