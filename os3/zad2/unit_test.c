#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <limits.h>

#define LOWER_BOUND = -100
#define UPPER_BOUND = 100

void error(char *message) {
    printf("%s\n", message);
    exit(1);
}

void match_boundaries(int* to_match, int lb, int up) {
    if (*to_match == INT_MIN)
        *to_match = *to_match + 1;
    *to_match = abs(*to_match);
    *to_match = (*to_match)%(up - lb + 1) + lb;
}

void write_to_file(int **matrix, int height, int width, int i, char* matrix_name) {
    int len = snprintf(NULL, 0, "m%d.%s", i, matrix_name) + 1;
    char path[len];
    snprintf(path, len, "m%d.%s", i, matrix_name);

    FILE* f = fopen(path, "w");
    if (f == NULL)
        error("Coudn't open file");

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width - 1; x++) 
            fprintf(f, "%d ", matrix[y][x]);
        if (y == height)
            fprintf(f, "%d", matrix[y][width - 1]);
        else
            fprintf(f, "%d\n", matrix[y][width - 1]);
    }
    fflush(f);
    fclose(f);
}


void get_matrix(int ***matrix, int height, int width, FILE* r_file) {
    for (int y = 0; y < height; y++) {
        (*matrix)[y] = calloc(width, sizeof(int));
        for (int x = 0; x < width; x++) {
            fread(&((*matrix)[y][x]), sizeof(int), 1, r_file);
            // no idea why changin -100 to LOWER_BOUND gives "expected an expression"
            match_boundaries(&((*matrix)[y][x]), -100, 100);
        }
    }
}


int main() {
    printf("unit\n");
    int tests_num = 5;

    FILE *r_file = fopen("/dev/random", "r");
    if (r_file == NULL)
        error("Coudn't open file rand");

    int min_matrix_len = 5;
    int max_matrix_len = 10;

    for (int test_num = 0; test_num < tests_num; test_num++) {
        // a_width == b_height
        int a_height, a_width, b_width;
        fread(&a_height, sizeof(int), 1, r_file);
        fread(&a_width, sizeof(int), 1, r_file);
        fread(&b_width, sizeof(int), 1, r_file);

        match_boundaries(&a_height, min_matrix_len, max_matrix_len);
        match_boundaries(&a_width, min_matrix_len, max_matrix_len);
        match_boundaries(&b_width, min_matrix_len, max_matrix_len);

        int **a_matrix = calloc(a_height, sizeof(int *));
        int **b_matrix = calloc(a_width, sizeof(int *));
        get_matrix(&a_matrix, a_height, a_width, r_file);
        get_matrix(&b_matrix, a_width, b_width, r_file);

        write_to_file(a_matrix, a_height, a_width, 0, "a");
        write_to_file(b_matrix, a_width, b_width, 0, "b");

        pid_t exec_child;
        if ((exec_child = fork()) == 0) {
            execl("./main", "./main", "lists", "10", "10", "1", NULL);
            exit(0);
        }
        int** c_matrix = calloc(a_height, sizeof(int*));

        for (int y = 0; y < a_height; y++) {
            c_matrix[y] = calloc(b_width, sizeof(int));
            for(int x = 0; x < b_width; x++) {
                c_matrix[y][x] = 0;
                for(int k = 0; k < a_width; k++) 
                    c_matrix[y][x] += a_matrix[y][k] * b_matrix[k][x];
            }
        }
        int status;
        wait(&status);

        FILE* f = fopen("m0c.txt", "r");
        if (f == NULL)
            error("Coudn't open file m0c");

        for (int y = 0; y < a_height; y++) {
            for (int x = 0; x < b_width; x++) {
                int t;
                fscanf(f, "%d", &t);
                if (t != c_matrix[y][x])
                    error("WRONG");
            }
        }
        fclose(f);

        // freeee
        for (int y = 0; y < a_height; y++) {
            free(c_matrix[y]);
            free(a_matrix[y]);
        }

        free(c_matrix);
        free(a_matrix);
        for (int x = 0; x < a_width; x++)
            free(b_matrix[x]);
        free(b_matrix);
    }
    printf("all passed\n");
    return 0;
}