#include "shared.h"


int move;
pthread_mutex_t move_mutex;
char* client_name;
int server_fd;
char sign;




void disconnect_from_server(){
	printf("Disconnecting from server...\n");
	send_msg(server_fd, DISCONNECT, NULL, NULL);
	if(shutdown(server_fd, SHUT_RDWR) < 0) 
		error("Cannot shutdown.");
	if(close(server_fd) < 0)
		error("Cannot close server descriptor.");
	exit(EXIT_SUCCESS);
}
void sigint_handler_client(int signo){
	disconnect_from_server();
}

void disconnect(){
	printf("Received DISCONNECT from server. Good bye!\n");
	sigint_handler_client(SIGINT);
	exit(EXIT_SUCCESS);
}
void connect_to_local_server(char* server){
	struct sockaddr_un addr;
	addr.sun_family = AF_UNIX;
	strcpy(addr.sun_path, server);
	server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
	
	if(server_fd < 0) 
		error("Socket to server failed.");
	if(connect(server_fd, (struct sockaddr*) &addr, sizeof(addr)) < 0) 
		error("Connect to server failed.");
}

void connect_to_inet_server(int port_number, char* server){
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port_number);
	addr.sin_addr.s_addr = inet_addr(server);
	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	
	if(server_fd < 0) 
		error("Socket to server failed.");
	if(connect(server_fd, (struct sockaddr*) &addr, sizeof(addr)) < 0) 
		error("Connect to server failed.");
}


void print_board(game* game){
	printf("BOARD\n");
	printf("Your sign: %c\n\n", sign);
	for(int i = 0; i < 9; i++){
		if (game -> board[i] == '-') 
			printf("%d", i);
		else 
			printf("%c", game->board[i]);

		if(i % 3 == 2) 
			printf("\n");
		else 
			printf (" ");
	}
	printf("\n");
}


void finish_game(game game){
	print_board(&game);
	if(game.winner == sign) 
		printf("Victory\n");
	else if(game.winner == 'D') 
		printf("Draw\n");
	else 
		printf("Defeat\n");
	disconnect_from_server();
	exit(EXIT_SUCCESS);
}


void enemy_move(void* arg){
	message* msg = (message*) arg;
	printf("Enter your move: ");

	int move_char = getchar();
	move = move_char - '0';

	while(move < 0 || move > 8 || msg->game.board[move] != '-'){
		move_char = getchar();
		move = move_char - '0';
	}
	pthread_exit(0);
}

void my_move(message *msg){
	move = -1;
	pthread_t move_thread;
	pthread_create(&move_thread, NULL, (void*) enemy_move, msg);

	while(1) {
		if (move < 0 || move > 8 || msg->game.board[move] != '-') {
			message receive_msg = receive_message(server_fd, 1);

			if (receive_msg.message_type == PING) 
				send_msg(server_fd, PING, NULL, NULL);
			else if (receive_msg.message_type == DISCONNECT) 
				disconnect();
		}
		else break;
	}

	pthread_join(move_thread, NULL);
	msg->game.board[move] = sign;
	print_board(&msg->game);
	send_msg(server_fd, MOVE, &msg->game, NULL);
}

void make_move(message msg){
	printf("your move\n");
	print_board(&msg.game);
	my_move(&msg);
}

void start_game(message msg){
	sign = msg.game.winner;
	printf("Your are: %c\n", sign);
	print_board(&msg.game);
	if(sign == 'O') 
		my_move(&msg);
	else printf("Waiting...\n");
}


void client_exec(){
	while(1){
		message msg = receive_message(server_fd, 0);
		if (msg.message_type == WAIT) 
			printf("Waiting...\n");
		else if (msg.message_type == PING) 
			send_msg(server_fd, PING, NULL, NULL);
		else if (msg.message_type == DISCONNECT) 
			disconnect();
		else if (msg.message_type == GAME_FOUND) 
			start_game(msg);
		else if (msg.message_type == MOVE) 
			make_move(msg);
		else if (msg.message_type == GAME_FINISHED) 
			finish_game(msg.game);
		else 
			break;
	}
}

void connect_with_server(){
	send_msg(server_fd, CONNECT, NULL, client_name);

	message msg = receive_message(server_fd, 0);

	if(msg.message_type == CONNECT){
		printf("connected to server\n");
		client_exec();
	}
	if(msg.message_type == CONNECT_FAILED){
		if(shutdown(server_fd, SHUT_RDWR) < 0) 
			error("shutdown.");
		if(close(server_fd) < 0) 
			error("close");
		exit(EXIT_FAILURE);
	}
}


int main(int argc, char** argv){

	if(argc < 4) 
		error("Wrong arguments: client_name connection_type serv_addres");
	srand(time(NULL));
	signal(SIGINT, sigint_handler_client);

	client_name = argv[1];

	connection_type connection;
	if(strcmp(argv[2], "LOCAL") == 0) 
		connection = LOCAL;
	else  
		connection = INET;

	char* server = argv[3];
	if(connection == LOCAL){
		connect_to_local_server(server);
	}
	else{
		if(argc < 5) 
			error("Wrong arguments. client_name connection_type addr port_num");
		printf("Server IP: %s port: %d\n",server, atoi(argv[4]));
		connect_to_inet_server(atoi(argv[4]), server);
	}
	connect_with_server();
	disconnect_from_server();
}