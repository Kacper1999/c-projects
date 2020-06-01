#include "shared.h"

void error(char* message){
	perror(message);
	exit(EXIT_FAILURE);
}

void new_board(game* game){
	for(int i = 0; i < 9; i++) game->board[i] = '-';
	game->turn = 'O';
	game->winner = '-';
}

void send_msg(int fd, message_type type, game* game, char* name){
	char* message = calloc(MSG_SIZE, sizeof(char));
	if(type == CONNECT) 
		sprintf(message, "%d %s", (int) type, name);
	else if(game == NULL) 
		sprintf(message, "%d", (int) type);
	else 
		sprintf(message, "%d %s %c %c", (int) type, game->board, game->turn, game->winner);

	if(write(fd, message, MSG_SIZE) < 0) 
		error("Cannot send message.");
	free(message);
}


message receive_message(int fd, int nonblocking){

	message msg;
	int count; 
	char* buffer = calloc(MSG_SIZE, sizeof(char));

	if (nonblocking && (count = recv(fd, buffer, MSG_SIZE, MSG_DONTWAIT)) < 0){ 
		msg.message_type = EMPTY;
		free(buffer);
		return msg;
	}
	else if(!nonblocking && (count = read(fd, buffer, MSG_SIZE)) < 0) error("Cannot receive message.");
	if(count == 0){
		msg.message_type = DISCONNECT;
		free(buffer);
		return msg;
	}

	char* token;
	char* rest = buffer;
	strcpy(msg.name, "");
	new_board(&msg.game);
	token = strtok_r(rest, " ", &rest);

	if (nonblocking) {
		char* p;
		msg.message_type = (message_type) strtol(token, &p, 10);
	}
	else msg.message_type = (message_type) atoi(token);

	if (msg.message_type == CONNECT){
		token = strtok_r(rest, " ", &rest);
		strcpy(msg.name, token);
	}
	else if (!nonblocking && (msg.message_type == PING || msg.message_type == DISCONNECT || msg.message_type == WAIT)){
		free(buffer);
		return msg;
	}
	else if (msg.message_type == MOVE || msg.message_type == GAME_FINISHED || msg.message_type == GAME_FOUND){
		token = strtok_r(rest, " ", &rest);
		strcpy(msg.game.board, token);
		token = strtok_r(rest, " ", &rest);
		msg.game.turn = token[0];
		token = strtok_r(rest, " ", &rest);
		msg.game.winner = token[0];
	}

	free(buffer);
	return msg;
}
