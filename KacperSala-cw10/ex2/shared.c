#include "shared.h"

void new_board(game* game) {
	for(int i = 0; i < 9; i++) game->board[i] = '-';
	game->turn = 'O';
	game->winner = '-';
}


void error(char* msg) {
	perror(msg);
	exit(EXIT_FAILURE);
}

void send_msg(int fd, msg_type type, game* game, char* nick){
	char* msg = calloc(MSG_SIZE, sizeof(char));
	if(game == NULL) 
		sprintf(msg, "%d %s", (int) type, nick);
	else
		sprintf(msg, "%d %s %c %c %s", (int) type, game->board, game->turn, game->winner, nick);
	if(write(fd, msg, MSG_SIZE) < 0)
		error("Could not send msg.");
	free(msg);
}

void send_msg_addr(int fd, struct sockaddr* addr, msg_type type, game* game, char* nick){
	char* msg = calloc(MSG_SIZE, sizeof(char));
	if(game == NULL)
		sprintf(msg, "%d %s", (int) type, nick);
	else
		sprintf(msg, "%d %s %c %c %s", (int) type, game->board, game->turn, game->winner, nick);
	if(sendto(fd, msg, MSG_SIZE, 0, addr, sizeof(struct sockaddr)) < 0)
		error("Could not send msg.");
	free(msg);
}

message rcv_msg_block(int fd){

	message msg;
	int count;
	char* msg_buf = calloc(MSG_SIZE, sizeof(char));

	if((count = read(fd, msg_buf, MSG_SIZE)) < 0) error("Could not receive msg.");
	if(count == 0){
		msg.msg_type = DISCONNECT;
		free(msg_buf);
		return msg;
	}

	char* token;
	char* rest = msg_buf;
	strcpy(msg.name, "");
	new_board(&msg.game);
	token = strtok_r(rest, " ", &rest);
	msg.msg_type = (msg_type) atoi(token);

	if (msg.msg_type == CONNECT){
		token = strtok_r(rest, " ", &rest);
		strcpy(msg.name, token);
	}
	else if (msg.msg_type == DISCONNECT || msg.msg_type == PING || msg.msg_type == WAIT){
		token = strtok_r(rest, " ", &rest);
		strcpy(msg.name, token);
		free(msg_buf);
		return msg;
	}
	else if (msg.msg_type == MOVE || msg.msg_type == GAME_FINISHED || msg.msg_type == GAME_FOUND){
		token = strtok_r(rest, " ", &rest);
		strcpy(msg.game.board, token);
		token = strtok_r(rest, " ", &rest);
		msg.game.turn = token[0];
		token = strtok_r(rest, " ", &rest);
		msg.game.winner = token[0];
		token = strtok_r(rest, " ", &rest);
		strcpy(msg.name, token);
	}

	free(msg_buf);

	return msg;

}

message rcv_msg_addr(int fd, struct sockaddr* addr, socklen_t len){

	message msg;
	int count;
	char* msg_buf = calloc(MSG_SIZE, sizeof(char));

	if ((count = recvfrom(fd, msg_buf, MSG_SIZE, 0, addr, &len)) < 0)
		error("Could not receive msg.");

	if (count == 0) {
		msg.msg_type=DISCONNECT;
		free(msg_buf);
		return msg;
	}

	char* token;
	char* rest = msg_buf;
	strcpy(msg.name, "");
	new_board(&msg.game);
	token = strtok_r(rest, " ", &rest);
	msg.msg_type = (msg_type) atoi(token);

	switch(msg.msg_type){
		case CONNECT:
			token = strtok_r(rest, " ", &rest);
			strcpy(msg.name, token);
			break;
		case PING: case WAIT: case DISCONNECT:
			token = strtok_r(rest, " ", &rest);
			strcpy(msg.name, token);
			free(msg_buf);
			return msg;
		case MOVE: case GAME_FOUND: case GAME_FINISHED:
			token = strtok_r(rest, " ", &rest);
			strcpy(msg.game.board, token);
			token = strtok_r(rest, " ", &rest);
			msg.game.turn = token[0];
			token = strtok_r(rest, " ", &rest);
			msg.game.winner = token[0];
			break;
		default:
			break;
	}

	free(msg_buf);

	return msg;

}

message rcv_msg(int fd){

	message msg;
	char* msg_buf = calloc(MSG_SIZE, sizeof(char));
	int count;

	if((count = recv(fd, msg_buf, MSG_SIZE, MSG_DONTWAIT)) < 0){
		msg.msg_type = EMPTY;
		free(msg_buf);
		return msg;
	}
	
	if(count == 0){
		msg.msg_type = DISCONNECT;
		free(msg_buf);
		return msg;
	}

	char *token;
	char *rest = msg_buf;

	char* p;

	strcpy(msg.name, "");
	new_board(&msg.game);
	token = strtok_r(rest, " ", &rest);

	msg.msg_type = (msg_type) strtol(token, &p, 10);

	switch (msg.msg_type) {
		case CONNECT:
			token = strtok_r(rest, " ", &rest);
			strcpy(msg.name, token);
			break;
		case PING: case WAIT: case DISCONNECT:
			token = strtok_r(rest, " ", &rest);
			strcpy(msg.name, token);
			free(msg_buf);
			return msg;
		case MOVE: case GAME_FOUND: case GAME_FINISHED:
			token = strtok_r(rest, " ", &rest);
			strcpy(msg.game.board, token);
			token = strtok_r(rest, " ", &rest);
			msg.game.turn = token[0];
			token = strtok_r(rest, " ", &rest);
			msg.game.winner = token[0];
			break;
		default:
			break;
	}

	free(msg_buf);

	return msg;

}