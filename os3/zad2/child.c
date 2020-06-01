#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/file.h>
#include <stdio.h>

int status;
int counter;
int idx;


void error(char *message) {
    printf("%s\n", message);
    exit(1);
}

void read_row(FILE *f, int **to_write, int n) {
    for (int i = 0; i < n; i++) 
        fscanf(f, "%d", &((*to_write)[i]));
}

void read_col(FILE *f, int **to_write, int idx, int n, int m) {
    int tmp;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            if (j != idx) 
                fscanf(f, "%d", &tmp);
            else 
                fscanf(f, "%d", &(*to_write)[i]);
        }
    }
}

int dot(int *a, int *b, int n) {
    int result = 0;
    for (int i = 0; i < n; i++)
        result += a[i] * b[i];
    return result;
}

void check_kill(int communication_fd, FILE *communication_f) {
    flock(communication_fd, LOCK_EX);
    fseek(communication_f, 2 * idx * sizeof(char), SEEK_SET);
    fscanf(communication_f, "%d", &status);
    if (status == 0) {
        flock(communication_fd, LOCK_UN);
        exit(counter);
    }
    flock(communication_fd, LOCK_UN);
}

void print_vec(int *vector, int n) {
    for (int i = 0; i < n; i++) 
        printf("%d ", vector[i]);
    printf("\n");
}


int main(int argc, char **argv) {
    printf("Child %s\n", argv[1]);
    idx = atoi(argv[1]);
    int children_num = atoi(argv[2]);

    int a_height = atoi(argv[5]);
    int a_width = atoi(argv[6]);
    int b_height = atoi(argv[8]);
    int b_width = atoi(argv[9]);
    char *mode = argv[11];

    FILE *a_matrix_f = fopen(argv[4], "r+");
    if (a_matrix_f == NULL) 
        error("can't open a matrix file");
        
    FILE *b_matrix_f = fopen(argv[7], "r+");
    if (b_matrix_f == NULL) 
        error("can't open b matrix file");

    FILE *communication_f = fopen(argv[3], "r+");
    while (communication_f == NULL) {
        communication_f = fopen(argv[3], "r+");
        usleep(1);
    }
    int communication_fd = fileno(communication_f);

    int last;
    char **buffer = (char **) calloc(a_height, sizeof(char *));
    size_t *lengths = (size_t *) calloc(a_height, sizeof(size_t));

    for (int i = 0; i < a_height; i++) {
        buffer[i] = NULL;
        lengths[i] = 0;
    }

    fseek(communication_f, 2 * idx * sizeof(char), SEEK_SET);
    fscanf(communication_f, "%d", &status);
    flock(communication_fd, LOCK_UN);

    int *b_col = calloc(b_height, sizeof(int));
    int *a_row = calloc(a_width, sizeof(int));
    int *c_col = calloc(a_height, sizeof(int));

    for (int i = idx; i < b_width; i += children_num) {
        check_kill(communication_fd, communication_f);
        counter++;

        fseek(b_matrix_f, 0, SEEK_SET);
        fseek(a_matrix_f, 0, SEEK_SET);
        read_col(b_matrix_f, &b_col, i, b_height, b_width);

        for (int j = 0; j < a_height; j++) {
            read_row(a_matrix_f, &a_row, a_width);
            c_col[j] = dot(a_row, b_col, a_width);
        }

        if (strcmp(mode, "0") == 0) {
            last = -2;
            while (last != i - 1) {
                check_kill(communication_fd, communication_f);
                flock(communication_fd, LOCK_EX);
                fseek(communication_f, 2 * children_num * sizeof(char), SEEK_SET);
                fscanf(communication_f, "%d", &last);
                flock(communication_fd, LOCK_UN);
            }

            FILE *f_C = fopen(argv[10], "r+");
            if(f_C == NULL){
                printf("Can't open output file\n");
                exit(counter);
            }
            flock(fileno(f_C), LOCK_EX);

            fseek(f_C, 0, SEEK_SET);
            for (int j = 0; j < a_height; j++) {
                char* b = NULL;
                size_t s = 0;

                lengths[j] = getline(&b, &s, f_C);
                buffer[j] = b;
                if(lengths[j] == EOF){
                    buffer[j] = NULL;
                    break;
                }
                if(lengths[j] == 0)
                    buffer[j][lengths[j]] = '\0';
                if (lengths[j] > 0) {
                    buffer[j][lengths[j]-1] = ' ';
                    buffer[j][lengths[j]] = '\0';
                }
            }
            fseek(f_C, 0, SEEK_SET);
            for (int j = 0; j < a_height; j++) {
                if (buffer[j] != NULL) 
                    fprintf(f_C, "%s%d\n", buffer[j], c_col[j]);
                else 
                    fprintf(f_C, "%d\n", c_col[j]);
                fflush(f_C);
            }
            fflush(f_C);
            fclose(f_C);

            for (int j = 0; j < a_height; j++) {
                free(buffer[j]);
                buffer[j] = NULL;
            }
            flock(fileno(f_C), LOCK_UN);
            fclose(f_C);
            flock(communication_fd, LOCK_EX);
            fseek(communication_f, 2 * children_num * sizeof(char), SEEK_SET);
            fprintf(communication_f, "%d\n", i);
            fflush(communication_f);
            flock(communication_fd, LOCK_UN);
        } else {
            int len = snprintf(NULL, 0, "/tmp/%d", i) + 1;
            char *path = calloc(len, sizeof(char));
            snprintf(path, len, "/tmp/%d", i);

            FILE *f = fopen(path, "w");
            if (f == NULL) {
                printf("Can't open file to write partial result\n");
                exit(counter);
            }
            for (int j = 0; j < a_height; j++) 
                fprintf(f, "%d\n", c_col[j]);

            fflush(f);
            fclose(f);
        }
    }
    fclose(communication_f);
    exit(counter);
}