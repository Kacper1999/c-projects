#include <sys/times.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/file.h>
#include <stdio.h>
#include <linux/limits.h>

#define COMMUNICATION_FILE "communicate.txt"

void error(char *message) {
    printf("%s\n", message);
    exit(1);
}

int read_from_file(FILE *f, char **to_write){
    *to_write = calloc(PATH_MAX, sizeof(char));
    if (fscanf(f, "%s\n", *to_write) == 0) {
        printf("Problem with reading line\n");
        return 0;
    }
    return 1;
}

int set_height_and_width(char* path, int* height, int* width){
    FILE* f = fopen(path, "r");
    if (f == NULL) {
        printf("Coudn't open file\n");
        return 1;
    }
    char* buffer = NULL;
    size_t len = 0;

    while (getline(&buffer, &len, f) != -1) {
        (*height)++;
        if (*height == 1) {
            char t[20];
            int offset = 0;

            while(sscanf(buffer + offset, "%s", t) != EOF) {
                offset += (strlen(t) + 1) * sizeof(char);
                (*width)++;
            }
        }
    }
    return 0;
}



int main(int argc, char **argv) {
    printf("main\n");

    if (argc != 5) 
        error("Invalid num of arguments");

    int children_num = atoi(argv[2]);
    pid_t children[children_num];

    char* a_fpath = NULL;
    char* b_fpath = NULL;
    char* c_fpath = NULL;

    if (get_matrices_paths(argv[1], &a_fpath, &b_fpath, &c_fpath))
        exit(1);

    int height_a = 0, width_a = 0;
    set_height_and_width(a_fpath, &height_a, &width_a);

    int height_b = 0, width_b = 0;
    set_height_and_width(b_fpath, &height_b, &width_b);

    int len = snprintf(NULL, 0, "%d", children_num) + 1;
    char str_children_num[len];
    snprintf(str_children_num, len, "%d", children_num);

    FILE* f = fopen(COMMUNICATION_FILE, "w+");
    flock(fileno(f), LOCK_EX);

    char* mode = argv[4];
    if (strcmp(mode, "1") && strcmp(mode, "0"))
        error("No matching mode");

    if (strcmp(mode, "0") == 0) {
        FILE* tm = fopen(c_fpath, "w+");
        fclose(tm);
    }

    for (int i = 0; i < children_num; i++) {
        fwrite("1\n", sizeof(char), 2, f);
        if ((children[i] = fork()) == 0) {
            len = snprintf(NULL, 0, "%d", i) + 1;
            char str_i[len];
            snprintf(str_i, len, "%d", i);

            len = snprintf(NULL, 0, "%d", height_a) + 1;
            char str_height_a[len];
            snprintf(str_height_a, len, "%d", height_a);

            len = snprintf(NULL, 0, "%d", width_a) + 1;
            char str_width_a[len];
            snprintf(str_width_a, len, "%d", width_a);

            len = snprintf(NULL, 0, "%d", height_b) + 1;
            char str_height_b[len];
            snprintf(str_height_b, len, "%d", height_b);

            len = snprintf(NULL, 0, "%d", width_b) + 1;
            char str_width_b[len];
            snprintf(str_width_b, len, "%d", width_b);

            // This looks terrible
            execl("./child", "./child", str_i, str_children_num, COMMUNICATION_FILE, a_fpath, str_height_a, str_width_a, b_fpath, str_height_b, str_width_b, c_fpath, mode, NULL);
        }
    }

    fwrite("-1\n", sizeof(char), 3, f);
    fflush(f);
    flock(fileno(f), LOCK_UN);

    int status = 0;
    int timeout = atoi(argv[3]) * 1000000;
    int time = 0;
    int sleep_time = 20000; // 20 ms
    int f_num = 0;

    for (int i = 0; i < children_num; i++) {
        while (time < timeout) {
            if (waitpid(children[i], &status, WNOHANG) != 0) {
                printf("Chiold PID %d did %d mlps\n", children[i], WEXITSTATUS(status));
                break;
            } else {
                usleep(sleep_time);
                time += sleep_time;
            }
        }

        if (time >= timeout) {
            flock(fileno(f), LOCK_EX);
            fseek(f, 2*i*sizeof(char), SEEK_SET);
            fprintf(f, "0\n");
            fflush(f);
            flock(fileno(f), LOCK_UN);
            waitpid(children[i], &status, 0);
            printf("Child  PID %d did %d mlps before diying\n", children[i], WEXITSTATUS(status));
        }
        f_num += WEXITSTATUS(status);
    }

    free(a_fpath);
    free(b_fpath);

    if (strcmp(mode, "1") == 0) {
        FILE* output_f = fopen(c_fpath, "w+");
        if (fork() == 0) {
            char** paramters = (char**) calloc(f_num + 4, sizeof(char*));
            paramters[0] = "/usr/bin/paste";
            paramters[1] = "-d";
            paramters[2] = " ";
            if (output_f == NULL) 
                error("failed trying to open output file");

            dup2(fileno(output_f), 1);
            int len;
            for (int i = 0; i < f_num; i++) {
                len = snprintf(NULL, 0, "/tmp/%d", i) + 1;
                paramters[i + 3] = calloc(len, sizeof(char));
                snprintf(paramters[i + 3], len, "/tmp/%d", i);
            }
            paramters[f_num + 3] = NULL;
            execv(paramters[0], paramters);
            exit(0);
        }
        wait(0);
    }
    return 0;
}