#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <time.h>
#include <sys/times.h>
#include <signal.h>

#define MAX_CMD_LEN 1024
#define MAX_ARG_NUM 20
#define MAX_CMDS_NUM 30

void error(char *message) {
    printf("%s\n", message);
    exit(EXIT_FAILURE);
}

char **parse(char* cmd){
    char **args = (char**) calloc(MAX_ARG_NUM+2, sizeof(char*));
    int argc = 0;
    char *arg;
    arg = strtok(cmd, " \n");
    while(arg != NULL){
        args[argc++] = arg;
        arg = strtok(NULL, " \n");
    }
    args[argc] = NULL;
    return args;
}

void execute(char *cmd, int mode) {
    static int prev_fd[2];
    static int curr_fd[2];

    if (pipe(curr_fd) != 0) 
        error("pipe failed");

    if (fork() == 0) {
        char **args = parse(cmd);
        if (mode != 1) {
            close(curr_fd[0]);
            if (dup2(curr_fd[1], STDOUT_FILENO) == -1)
                error("dup2 Failed");
        }
        if (mode != -1) {
            close(prev_fd[1]);
            if (dup2(prev_fd[0], STDIN_FILENO) == -1)
                error("dup2 Failed");
        }
        if (execvp(args[0], args) == -1) 
            error("Executing cmd failed");
    }

    if (mode == 0) {
        close(prev_fd[0]);
        close(prev_fd[1]);
    }
    if (mode != 1) {
        prev_fd[0] = curr_fd[0];
        prev_fd[1] = curr_fd[1];
    } else {
        close(curr_fd[0]);
        close(curr_fd[1]);
    }
}

void exec_line(char line[MAX_CMD_LEN]) {
    int coms = 0;
    char **cmds = (char **) calloc(MAX_CMDS_NUM, sizeof(char *));
    char *cmd;

    cmd = strtok(line, "|");
    while (cmd != NULL) {
        cmds[coms++] = cmd;
        cmd = strtok(NULL, "|");
    }

    int mode = 0;

    for (int i = 0; i < coms; i++) {
        if (i == 0)
            mode = -1;
        else if (i == coms - 1)
            mode = 1;
        else 
            mode = 0;
        execute(cmds[i], mode);
    }

    wait(NULL);
}


int main(int argc, char **argv) {
    if (argc != 2) 
        error("Wrong number of arguments");
    char *file_path = argv[1];
    FILE *f = fopen(file_path, "r");
    if (f == NULL) 
        error("Coudn't open file");

    char cmd[MAX_CMD_LEN];
    while (fgets(cmd, MAX_CMD_LEN, f) != NULL) 
        exec_line(cmd);
    fclose(f);
    return 0;
}
