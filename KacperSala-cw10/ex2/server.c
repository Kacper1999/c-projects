#include "shared.h"

int port_number;
char* socket_path;


pthread_mutex_t clients_mutex;
pthread_t net_thread;
pthread_t ping_thread;

client clients[MAX_CLIENTS];
int waiting_idx;
int first_free;
struct sockaddr local_sockaddr;
int local_sock;

struct sockaddr_in inet_sockaddr;
int inet_sock;



void sigint_handler_server(int signo){
	exit(EXIT_SUCCESS);
}
void close_server(){
	if(pthread_cancel(net_thread) == -1) 
		error("net tread cancle");
	if(pthread_cancel(ping_thread) == -1)
		error("ping thread cancle");
	close(local_sock);
	unlink(socket_path);
	close(inet_sock);
}
void exit_fun(){
	close_server();
}
void empty_client(int i){
	if(clients[i].name != NULL) 
		free(clients[i].name);
	if(clients[i].addr != NULL) 
		free(clients[i].addr);
	clients[i].name = NULL;
	clients[i].addr = NULL;
	clients[i].fd = -1;
	clients[i].game = NULL;
	clients[i].active = 0;
	clients[i].symbol = '-';
	clients[i].enemy_i = -1;
	if(waiting_idx == i)
		waiting_idx = -1;
}

int client_exist(int i){
	return i>= 0 && i < MAX_CLIENTS && clients[i].fd != -1;
}

int get_free_idx(){
	for(int i = 0; i < MAX_CLIENTS; i++)
		if(!client_exist(i)) 
			return i;
	return -1;
}

int get_client_index(char* name){
	for(int i = 0; i < MAX_CLIENTS; i++)
		if(client_exist(i) && strcmp(name, clients[i].name) == 0) 
			return i;
	return -1;
}

int is_name_available(char* name){
	for(int i = 0; i < MAX_CLIENTS; i++)
		if(client_exist(i) && strcmp(name, clients[i].name) == 0) 
			return 0;

	return 1;
}

void start(){
	local_sock = socket(AF_UNIX, SOCK_DGRAM, 0);
	if(local_sock == -1) 
		error("local socket");

	local_sockaddr.sa_family = AF_UNIX;
	strcpy(local_sockaddr.sa_data, socket_path);
	if (bind(local_sock, &local_sockaddr, sizeof(local_sockaddr)) == -1) 
		error("local bind");

	printf("Local socket fd: %d\n", local_sock);

	inet_sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(inet_sock == -1) 
		error("ined socket");
	inet_sockaddr.sin_family = AF_INET;
	inet_sockaddr.sin_port = htons(port_number);
	inet_sockaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	if(bind(inet_sock, (struct sockaddr*) &inet_sockaddr, sizeof(inet_sockaddr)) == -1)
		error("inet bind");
}

void start_game(int id1, int id2){

	clients[id1].enemy_i = id2;
	clients[id2].enemy_i = id1;

	if (rand() % 2 == 1){
		clients[id1].symbol = 'O';
		clients[id2].symbol = 'X';
	}
	else{
		clients[id1].symbol = 'X';
		clients[id2].symbol = 'O';
	}

	game* game = malloc(sizeof(game));
	new_board(game);

	clients[id1].game = clients[id2].game = game;
	game->winner = clients[id1].symbol;
	send_msg_addr(clients[id1].fd, clients[id1].addr, GAME_FOUND, game, clients[id1].name);
	game->winner = clients[id2].symbol;
	send_msg_addr(clients[id2].fd, clients[id2].addr, GAME_FOUND, game, clients[id2].name);
	game->winner = '-';
}

void connect_client(int fd, struct sockaddr* addr, char* new_nick){
	printf("Connecting to server\n");

	char* nick = calloc(NAME_LEN, sizeof(char));
	strcpy(nick, new_nick);

	if(is_name_available(nick) == 0){
		send_msg_addr(fd, addr, CONNECT_FAILED, NULL, "name taken");
		free(nick);
		return;
	}
	if(first_free == -1){
		send_msg_addr(fd, addr, CONNECT_FAILED, NULL, "No space");
		free(nick);
		return;
	}

	send_msg_addr(fd, addr, CONNECT, NULL, "Connected");

	clients[first_free].name = nick;
	clients[first_free].active = 1;
	clients[first_free].fd = fd;
	clients[first_free].addr = addr;


	for(int i = 0; i < MAX_CLIENTS; i++){
		if(clients[i].name != NULL) printf("%d: %s\n",i, clients[i].name);
	}

	if(waiting_idx != -1){
		start_game(first_free, waiting_idx);
		waiting_idx = -1;
	}
	else{
		waiting_idx = first_free;
		send_msg_addr(fd, clients[first_free].addr, WAIT, NULL, nick);
	}

	first_free=get_free_idx();

	printf("Client connected\n");
}

void check_game_status(game* game){
	static int wins[8][3] = { {0, 1, 2}, {3, 4, 5}, {6, 7, 8}, {0, 3, 6}, {1, 4, 7}, {2, 5, 8}, {0, 4, 8}, {2, 4, 6} };

	for(int i = 0; i < 8; i++){
		if (game->board[wins[i][0]] == game->board[wins[i][1]] && game->board[wins[i][1]] == game->board[wins[i][2]])
		{
			game->winner = game->board[wins[i][0]];
			return;
		}
	}

	int is_move = 0;
	for(int i = 0; i < 9; i++){
		if(game->board[i] == '-'){
			is_move = 1;
			break;
		}
	}
	if(is_move == 0) 
		game->winner = 'D';
	if(game->turn == 'X') 
		game->turn = 'O';
	else if(game->turn == 'O') 
		game->turn = 'X';
}

void net_exec(void* arg){

	struct pollfd poll_fds[2];

	poll_fds[0].fd = local_sock;
	poll_fds[1].fd = inet_sock;
	poll_fds[0].events = POLLIN;
	poll_fds[1].events = POLLIN;

	while(1){

		pthread_mutex_lock(&clients_mutex);
		for (int i = 0; i < 2; i++) {
			poll_fds[i].events = POLLIN;
			poll_fds[i].revents = 0;
		}
		pthread_mutex_unlock(&clients_mutex);
		if(poll(poll_fds, 2, -1) == -1) error("Cannot poll");
		pthread_mutex_lock(&clients_mutex);

		for(int i = 0; i < 2; i++){
			if(poll_fds[i].revents && POLLIN){

				struct sockaddr* addr = malloc(sizeof(struct sockaddr));
				socklen_t len = sizeof(&addr);
				printf("fd: %d\n", poll_fds[i].fd);

				message msg = rcv_msg_addr(poll_fds[i].fd, addr, len);
				printf("msg received\n");
				int idx;

				if (msg.msg_type == CONNECT){
					connect_client(poll_fds[i].fd, addr, msg.name);
				}

				else if (msg.msg_type == MOVE){

					printf("Received move from client\n");
					idx = get_client_index(msg.name);
					check_game_status(&msg.game);

					if(msg.game.winner == '-') {
						send_msg_addr(clients[clients[idx].enemy_i].fd, clients[clients[idx].enemy_i].addr, MOVE, &msg.game, clients[clients[idx].enemy_i].name);
					}

					else{
						send_msg_addr(clients[idx].fd, clients[idx].addr, GAME_FINISHED, &msg.game, clients[idx].name);
						send_msg_addr(clients[clients[idx].enemy_i].fd, clients[clients[idx].enemy_i].addr, GAME_FINISHED, &msg.game, clients[clients[idx].enemy_i].name);
						free(clients[idx].game);
					}
					free(addr);
				}

				else if (msg.msg_type == DISCONNECT){

					idx = get_client_index(msg.name);
					printf("Received\n");

					if(client_exist(clients[idx].enemy_i)){
						send_msg_addr(clients[clients[idx].enemy_i].fd, clients[clients[idx].enemy_i].addr, DISCONNECT, NULL, clients[clients[idx].enemy_i].name);
						empty_client(clients[idx].enemy_i);
					}

					empty_client(idx);
					free(addr);
				}

				else if (msg.msg_type == PING){
					idx = get_client_index(msg.name);
					clients[idx].active = 1;
					free(addr);
				}

				else free(addr);
				
			}
		}
		pthread_mutex_unlock(&clients_mutex);
	}
}

void ping_exec(void* arg){
	while(1){

		sleep(PING_INTERVAL);
		printf("serv wating ...\n");
		pthread_mutex_lock(&clients_mutex);

		for(int i = 0; i < MAX_CLIENTS; i++){
			if(client_exist(i)){
				clients[i].active = 0;
				send_msg_addr(clients[i].fd, clients[i].addr, PING, NULL, clients[i].name);
			}
		}

		pthread_mutex_unlock(&clients_mutex);
		sleep(PING_WAIT);
		pthread_mutex_lock(&clients_mutex);

		for(int i = 0; i < MAX_CLIENTS; i++){

			if(client_exist(i) && clients[i].active == 0) {
				printf("Client %d didn't respond. Disconnecting %d...\n", i, i);
				send_msg_addr(clients[i].fd, clients[i].addr, DISCONNECT, NULL, clients[i].name);
				
				if(client_exist(clients[i].enemy_i)){
					send_msg_addr(clients[clients[i].enemy_i].fd, clients[clients[i].enemy_i].addr, DISCONNECT, NULL, clients[clients[i].enemy_i].name);
					empty_client(clients[i].enemy_i);
				}
				empty_client(i);
			}
		}

		pthread_mutex_unlock(&clients_mutex);
	}
}



int main(int argc, char** argv){

	if(argc < 3) 
		error("Wrong arguments: port socket_path");
	srand(time(NULL));

	port_number = atoi(argv[1]);
	socket_path = argv[2];
	signal(SIGINT, sigint_handler_server);
	atexit(exit_fun);

	for(int i = 0; i < MAX_CLIENTS; i++) empty_client(i);

	start();
	waiting_idx = -1;
	first_free = 0;

	if(pthread_create(&net_thread, NULL, (void*) net_exec, NULL) == -1) 
		error("net thread.");
	if(pthread_create(&ping_thread, NULL, (void*) ping_exec, NULL) == -1) 
		error("ping thread");


	if(pthread_join(net_thread, NULL) < 0) 
		error("net thread.");
	if(pthread_join(ping_thread, NULL) < 0) 
		error("ping thread.");

	close_server();
	exit(EXIT_SUCCESS);
}