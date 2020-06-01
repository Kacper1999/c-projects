#include "shared.h"
#include <signal.h>

int   id;
char* name;

int   serv_q_desc;
int   client_q_desc;

int my_chat_q_desc = -1;


void register_me() {
    char csMsg[MAX_MSG_LENGTH];
    sprintf(csMsg, "%d %s", CLIENT_SERV_INIT, name);

    SEND_MSG(serv_q_desc, csMsg, CLIENT_SERV_INIT);

    char scMsg[MAX_MSG_LENGTH];
    unsigned int type;
    RECEIVE_MSG(client_q_desc, scMsg, &type);

    sscanf(scMsg, "%d %d", &type, &id);
    printf("client: registered with id: %d, key: %s\n", id, name);
}

void send_stop() {
    printf("client: sending STOP..\n");

    char csMsg[MAX_MSG_LENGTH];
    sprintf(csMsg, "%d %d", CLIENT_SERV_STOP, id);
    SEND_MSG(serv_q_desc, csMsg, CLIENT_SERV_STOP);

    exit_client();
}


void send_disconnect() {
    printf("client: disconnected chat\n");

    char csMsg[MAX_MSG_LENGTH];
    sprintf(csMsg, "%d %d", CLIENT_SERV_DISCONNECT, id);
    SEND_MSG(serv_q_desc, csMsg, CLIENT_SERV_DISCONNECT);

    if (my_chat_q_desc != -1) {
        char ccMsg[MAX_MSG_LENGTH];
        sprintf(csMsg, "%d %d", CLIENT_CLIENT_DICONNECT, id);
        SEND_MSG(my_chat_q_desc, ccMsg, CLIENT_CLIENT_DICONNECT);

        CLOSE_Q(my_chat_q_desc);
        my_chat_q_desc = -1;
    }
}
void sendList() {
    printf("client: sending LIST..\n");

    char csMsg[MAX_MSG_LENGTH];
    sprintf(csMsg, "%d %d", CLIENT_SERV_LIST, id);
    SEND_MSG(serv_q_desc, csMsg, CLIENT_SERV_LIST);
}

void send_connect(int my_chatId) {
    printf("client: seending CONNECT to my_chat with ID: %d\n", my_chatId);

    char csMsg[MAX_MSG_LENGTH];
    sprintf(csMsg, "%d %d %d", CLIENT_SERV_CONNECT, id, my_chatId);
    SEND_MSG(serv_q_desc, csMsg, CLIENT_SERV_CONNECT);
}

void send_msg(char* msg) {
    char ccMsg[MAX_MSG_LENGTH];
    sprintf(ccMsg, "%d %s", CLIENT_CLIENT_MSG, msg);

    SEND_MSG(my_chat_q_desc, ccMsg, CLIENT_CLIENT_MSG);
      printf("Me:          \t%s\n", msg);
}

void handle_terminate() {
    printf("client: Received terminate signal.. Server is wating for STOP..\n");
}
void handle_disconnect(char* msg) {
    printf("client: received disconnect msg from my_chat..\n");

    my_chat_q_desc = -1;
    CLOSE_Q(my_chat_q_desc);
    send_disconnect();
}
void handle_msg(char* ccMsg) {
    char msg[MAX_MSG_LENGTH];
    unsigned int type;
    sscanf(ccMsg, "%d %s", &type, msg);

    printf("Chatee says:\t%s\n", msg);
}
void handle_init(char* msg) {
    int          my_chatId;
    unsigned int type;
    char         my_chatName[MAX_MSG_LENGTH];
    sscanf(msg, "%d %d %s", &type, &my_chatId, my_chatName);
    printf("client: entering chat with %d..\n", my_chatId);
    my_chat_q_desc = GET_Q(my_chatName);
}

void handle_exit_sig(int sig) { send_stop(); }



void register_notification() {
    struct sigevent ev;
    ev.sigev_notify = SIGEV_SIGNAL;
    ev.sigev_signo = SIGUSR1;
    REGISTER_NOTIFICATION(client_q_desc, &ev);
}

void handle_sig(int signal) {
    char* msg = malloc(sizeof(char) * MAX_MSG_LENGTH);
    unsigned int type;

    RECEIVE_MSG(client_q_desc, msg, &type);
    if (type == SERV_CLIENT_CHAT_INIT) 
        handle_init(msg);
    else if (type == SERV_CLIENT_TERMINATE) 
        handle_terminate();
    else if (type == CLIENT_CLIENT_MSG) 
        handle_msg(msg);
    else if (type == CLIENT_CLIENT_DICONNECT) 
        handle_disconnect(msg);
    else 
        printf("client: received msg of unknown type..\n");
    free(msg);
    register_notification();
}
void exit_client() {
    printf("client: exit..\n");
    CLOSE_Q(serv_q_desc);
    CLOSE_Q(client_q_desc);
    DELETE_Q(name);
    exit(EXIT_SUCCESS);
}
int main(int charc, char* argv[]) {
    srand(time(NULL));
    name = Q_CLIENT_RANDOM_NAME;
    client_q_desc = CREATE_Q(name);
    if (client_q_desc == -1) {
        printf("There already exists client associated with this queue...\n");
        error();
        return -1;
    }
    serv_q_desc = GET_Q(Q_SERV_NAME);
    if (serv_q_desc == -1) {
        printf("Failed to open serv queue\n");
        error();
    }
    signal(SIGINT, handle_exit_sig);
    signal(SIGUSR1, handle_sig);
    register_me();
    register_notification();
    char buffer[MAX_MSG_LENGTH];
    char msg[MAX_MSG_LENGTH];

    while (1) {
        scanf("%s", buffer);
        if (str_equal(buffer, "STOP")) 
            send_stop();
        else if (str_equal(buffer, "LIST")) 
            sendList();
        else if (str_equal(buffer, "DISCONNECT"))
            send_disconnect();
        else if (str_equal(buffer, "CONNECT")) {
            int my_chatId;
            scanf("%d", &my_chatId);
            send_connect(my_chatId);
        } else if (str_equal(buffer, "SEND")) {
            scanf("%s", msg);
            if (my_chat_q_desc == -1) 
                printf("client: unable to send msg\n");
            else
                send_msg(msg);
        }
    }
    return 0;
}