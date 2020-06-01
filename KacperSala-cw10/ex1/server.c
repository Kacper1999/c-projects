#include "shared.h"

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
int client_exist(int i){
	return i > -1 && i < MAX_CLIENTS && clients[i].fd != -1;
}

void disconnect_client(int i){
	printf("Disconnecting...%s\n", clients[i].name);
	if(!client_exist(i)) 
		return;
	if(shutdown(clients[i].fd, SHUT_RDWR) < 0) 
		error("client shutdown");

	if(close(clients[i].fd) < 0)
		error("client close");
}


void empty_client(int i){
	if(clients[i].name != NULL) 
		free(clients[i].name);
	clients[i].name = NULL;
	clients[i].fd = -1;
	clients[i].game = NULL;
	clients[i].active = 0;
	clients[i].symbol = '-';
	clients[i].opponent_idx = -1;
	if(waiting_idx == i)
		waiting_idx = -1;
}
void start(int port_number){
	local_sock = socket(AF_UNIX, SOCK_STREAM, 0); 
	if(local_sock == -1) 
		error("local socket");

	local_sockaddr.sa_family = AF_UNIX;
	strcpy(local_sockaddr.sa_data, socket_path);

	if(bind(local_sock, &local_sockaddr, sizeof(local_sockaddr)) < 0) 
		error("Local bind"); 
	if(listen(local_sock, MAX_CLIENTS) < 0) 
		error("Local listen"); 

	printf("Local socket fd: %d\n", local_sock);

	inet_sock = socket(AF_INET, SOCK_STREAM, 0);
	if(inet_sock == -1) 
		error("Inet socket");

	inet_sockaddr.sin_family = AF_INET;
	inet_sockaddr.sin_port = htons(port_number);
	inet_sockaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	if(bind(inet_sock, (struct sockaddr*) &inet_sockaddr, sizeof(inet_sockaddr)) < 0) 
		error("Inet bind");
	if(listen(inet_sock, MAX_CLIENTS) < 0) 
		error("Inet listen");
}
int first_free_idx(){
	for(int i = 0; i < MAX_CLIENTS; i++){
		if(!client_exist(i)) return i;
	}
	return -1;
}

int is_name_free(char* name){
	for(int i = 0; i < MAX_CLIENTS; i++){
		if(client_exist(i) && strcmp(name, clients[i].name) == 0) return 0;
	}
	return 1;
}

void check_game_status(game* game){
	int wins[8][3] = { {0, 1, 2}, {3, 4, 5}, {6, 7, 8}, {0, 3, 6}, {1, 4, 7}, {2, 5, 8}, {0, 4, 8}, {2, 4, 6} };

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
	if(!is_move) game->winner = 'D';

	if(game->turn == 'X') game->turn = 'O';
	else if(game->turn == 'O') game->turn = 'X';
}



void start_game(int id1, int id2){

	clients[id1].opponent_idx = id2;
	clients[id2].opponent_idx = id1;

	if (rand() % 2 == 1){
		clients[id1].symbol = 'O';
		clients[id2].symbol = 'X';
	}
	else{
		clients[id1].symbol = 'X';
		clients[id2].symbol = 'O';
	}

	game* new_game = malloc(sizeof(game));
	new_board(new_game);

	clients[id1].game = clients[id2].game = new_game;

	new_game->winner = clients[id1].symbol;
	send_msg(clients[id1].fd, GAME_FOUND, new_game, NULL);

	new_game->winner = clients[id2].symbol;
	send_msg(clients[id2].fd, GAME_FOUND, new_game, NULL);

	new_game->winner = '-';
}

void connect_client(int fd){
	printf("Connecting client\n");

	int client_fd = accept(fd, NULL, NULL);
	if(client_fd < 0) 
		error("Could not accept client.");

	message msg = receive_message(client_fd, 0);

	char* name = calloc(NAME_LEN, sizeof(char));

	strcpy(name, msg.name);

	if(!is_name_free(name)){
		send_msg(client_fd, CONNECT_FAILED, NULL, "name taken");
		return;
	}
	if(first_free == -1){
		send_msg(client_fd, CONNECT_FAILED, NULL, "no space");
	}

	send_msg(client_fd, CONNECT, NULL, "Connected");

	clients[first_free].name = name;
	clients[first_free].active = 1;
	clients[first_free].fd = client_fd;

	for(int i = 0; i < MAX_CLIENTS; i++)
		if(clients[i].name != NULL) printf("%d: %s\n",i, clients[i].name);

	if(waiting_idx != -1){
		start_game(first_free, waiting_idx);
		waiting_idx = -1;
	}
	else{
		waiting_idx = first_free;
		send_msg(client_fd, WAIT, NULL, NULL);
	}

	first_free = first_free_idx();
	printf("Connected\n");
}



void net_exec(void* arg){

	struct pollfd poll_fds[MAX_CLIENTS + 2]; 

	poll_fds[MAX_CLIENTS].fd = local_sock;
	poll_fds[MAX_CLIENTS + 1].fd = inet_sock;


	while(1) {

		pthread_mutex_lock(&clients_mutex);

		for (int i = 0; i < MAX_CLIENTS + 2; i++) {
			if(i < MAX_CLIENTS) poll_fds[i].fd = clients[i].fd; 
			poll_fds[i].events = POLLIN; 
			poll_fds[i].revents = 0; 
		}

		pthread_mutex_unlock(&clients_mutex);

		if(poll(poll_fds, MAX_CLIENTS + 2, -1) == -1) 
			error("poll"); 

		pthread_mutex_lock(&clients_mutex);

		for(int i = 0; i < MAX_CLIENTS + 2; i++){
			if(i < MAX_CLIENTS && !client_exist(i)) continue;

			if(poll_fds[i].revents && POLLIN){ 

				if(poll_fds[i].fd == local_sock || poll_fds[i].fd == inet_sock){
					connect_client(poll_fds[i].fd);
				}

				else{

					message msg = receive_message(poll_fds[i].fd, 0);
					if (msg.message_type == MOVE){

						printf("Received move\n");
						check_game_status(&msg.game);

						if(msg.game.winner == '-') {
							send_msg(clients[clients[i].opponent_idx].fd, MOVE, &msg.game, NULL);
						}

						else{
							send_msg(poll_fds[i].fd, GAME_FINISHED, &msg.game, NULL);
							send_msg(clients[clients[i].opponent_idx].fd, GAME_FINISHED, &msg.game, NULL);
							free(clients[i].game);
						}
					}

					else if (msg.message_type == DISCONNECT){
						printf("client disconnect\n");

						if(client_exist(clients[i].opponent_idx)){
							disconnect_client(clients[i].opponent_idx);
							empty_client(clients[i].opponent_idx);
						}

						disconnect_client(i);
						empty_client(i);
					}

					else if (msg.message_type == PING) clients[i].active = 1;
				}
			}
			else if(client_exist(i) && poll_fds[i].revents && POLLHUP){ 
				printf("Disconnecting...\n");
				disconnect_client(i);
				empty_client(i);
			}
		}

		pthread_mutex_unlock(&clients_mutex);
	}
}



void ping_exec(void* arg){
	while(1){

		sleep(PING_INTERVAL);
		printf("waiting ...\n");

		pthread_mutex_lock(&clients_mutex);

		for(int i = 0; i < MAX_CLIENTS; i++){
			if(client_exist(i)){
				clients[i].active = 0;
				send_msg(clients[i].fd, PING, NULL, NULL);
			}
		}

		pthread_mutex_unlock(&clients_mutex);

		sleep(PING_WAIT);

		pthread_mutex_lock(&clients_mutex);

		for(int i = 0; i < MAX_CLIENTS; i++){
			if(client_exist(i) && clients[i].active == 0) {

				printf("Client %s didn't respond. Disconnecting %d...\n", clients[i].name, i);
				send_msg(clients[i].fd, DISCONNECT, NULL, NULL);

				if(client_exist(clients[i].opponent_idx)){
					disconnect_client(clients[i].opponent_idx);
					empty_client(clients[i].opponent_idx);
				}
				
				disconnect_client(i);
				empty_client(i);
			}
		}

		pthread_mutex_unlock(&clients_mutex);
		printf("ending\n");
	}

}



int main(int argc, char** argv){

	if(argc < 3)
		error("Wrong number of arguments. Expected: [number of port] [path to socket]");
	signal(SIGINT, sigint_handler_server);

	int port_number = atoi(argv[1]);
	socket_path = argv[2];

	atexit(close_server);

	for(int i = 0; i < MAX_CLIENTS; i++) empty_client(i);

	start(port_number);
	waiting_idx = -1;
	first_free = 0;

	if(pthread_create(&net_thread, NULL, (void*) net_exec, NULL) == -1) 
		error("net thread create");
	if(pthread_create(&ping_thread, NULL, (void*) ping_exec, NULL) == -1) 
		error("ping thread create");


	if(pthread_join(net_thread, NULL) < 0) 
		error("net thread join");
	if(pthread_join(ping_thread, NULL) < 0) 
		error("ping thread join");

	close_server();
	exit(EXIT_SUCCESS);
}