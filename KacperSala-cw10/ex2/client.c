#include "shared.h"

connection_type connection;
int port_number;
char* server;
char* name;
int server_fd;
char sign;
int move;

void disconnect_from_server(){
	printf("Disconnecting from server...\n");
	send_msg(server_fd, DISCONNECT, NULL, name);
	if(connection == LOCAL) unlink(name);
}

void sigint_handler_client(int signo){
	exit(EXIT_SUCCESS);
}

void exit_fun(){
	disconnect_from_server();
}

void print_gameboard(game* game){
	printf("Tic-Tac-Tow\n");
	printf("You are: %c\n\n", sign);

	for(int i = 0; i < 9; i++){
		if (game -> board[i] == '-') printf("%d", i);
		else printf("%c",game->board[i]);

		if(i % 3 == 2) printf("\n");
		else printf (" ");
	}

	printf("\n");
}

void connect_to_inet_server(){
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port_number);
	addr.sin_addr.s_addr = inet_addr(server);

	server_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (server_fd < 0) 
		error("inet server_fd");

	struct sockaddr_in c_addr;
	c_addr.sin_family = AF_INET;
	c_addr.sin_port = 0;
	c_addr.sin_addr.s_addr = inet_addr(server);

	if(bind(server_fd, (struct sockaddr*) &c_addr, sizeof(c_addr)) < 0) 
		error("inet bind");
	if(connect(server_fd, (struct sockaddr*) &addr, sizeof(addr)) < 0) 
		error("inet connect");
}

void connect_to_local_server(){
	struct sockaddr_un addr;
	addr.sun_family = AF_UNIX;
	strcpy(addr.sun_path, server);

	server_fd = socket(AF_UNIX, SOCK_DGRAM, 0);
	if(server_fd < 0) 
		error("local server_fd.");

	struct sockaddr_un c_addr;
	c_addr.sun_family = AF_UNIX;
	strcpy(c_addr.sun_path, name);

	if(bind(server_fd, (struct sockaddr*) &c_addr, sizeof(c_addr)) < 0) 
		error("local bind");
	if(connect(server_fd, (struct sockaddr*) &addr, sizeof(addr)) < 0) 
		error("local connect");
}



void enemy_move(void* arg){
	message* msg = (message*) arg;
	printf("Your move: ");

	int move_char = getchar();
	move = move_char - '0';

	while(move < 0 || move > 8 || msg->game.board[move] != '-'){
		move_char = getchar();
		move = move_char - '0';
	}
	pthread_exit(0);
}

void make_move(message *msg){
	move = -1;
	pthread_t move_thread;
	pthread_create(&move_thread, NULL, (void*) enemy_move, msg);

	while(1){
		if(move < 0 || move > 8 || msg->game.board[move] != '-'){
			message rec_msg = rcv_msg(server_fd);

			if (rec_msg.msg_type == DISCONNECT){
				sigint_handler_client(SIGINT);
				exit(EXIT_SUCCESS);
			} else if (rec_msg.msg_type == PING)
				send_msg(server_fd, PING, NULL, name);
		}
		else
			break;
	}

	pthread_join(move_thread, NULL);
	msg->game.board[move] = sign;
	print_gameboard(&msg->game);
	send_msg(server_fd, MOVE, &msg->game, name);
}

void client_exec(){
	while(1){
		message msg = rcv_msg_block(server_fd);
		
		if (msg.msg_type == WAIT) 
			printf("Waiting...\n");
		else if (msg.msg_type == PING) 
			send_msg(server_fd, PING, NULL, name);
		else if (msg.msg_type == DISCONNECT) 
			exit(EXIT_SUCCESS);
		else if (msg.msg_type == GAME_FOUND){
			sign = msg.game.winner;
			printf("You are: %c\n", sign);
			print_gameboard(&msg.game);
			if(sign == 'O') 
				make_move(&msg);
			else 
				printf("Waiting...\n");
		}
		else if (msg.msg_type == MOVE){
			print_gameboard(&msg.game);
			make_move(&msg);
		}
		else if (msg.msg_type == GAME_FINISHED){
			print_gameboard(&msg.game);
			if(msg.game.winner == sign) 
				printf("Vicotry\n");
			else if(msg.game.winner == 'D') 
				printf("Draw\n");
			else 
				printf("Defeat\n");
			exit(EXIT_SUCCESS);
		}
		else break;
	}
}

int main(int argc, char** argv){

	if(argc < 4) 
		error("Wrong arguments: client_name connection_typ address");

	name = argv[1];
	atexit(exit_fun);
	signal(SIGINT, sigint_handler_client);

	if(strcmp(argv[2], "LOCAL") == 0) 
		connection = LOCAL;
	else if(strcmp(argv[2], "INET") == 0) 
		connection = INET;
	else 
		error("Wrong connection: LOCAL / INET");

	if (connection == LOCAL){
		server = argv[3];
		connect_to_local_server();
	}
	else{
		if(argc < 5) 
			error("Wrong arguments: client_name connection_typ adress port_num");
		server = argv[3];
		port_number = atoi(argv[4]);
		connect_to_inet_server();
	}

	send_msg(server_fd, CONNECT, NULL, name);
	message msg = rcv_msg_block(server_fd);

	if(msg.msg_type == CONNECT){
		printf("Client connected\n");
		client_exec();
	}
	if(msg.msg_type == CONNECT_FAILED){
		printf("Cannot connect %s\n",msg.name);
		if(shutdown(server_fd, SHUT_RDWR) < 0)
			error("shutdown");
		if(close(server_fd) < 0)
			error("close");
		exit(EXIT_FAILURE);
	}
	
	disconnect_from_server();
}