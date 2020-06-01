#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <sys/times.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <limits.h>

FILE* r_stream;
// a lot of copy and pasting from other files but I dont feel like doing a header
void error(char *message) {
    printf("%s\n", message);
    exit(1);
}

void match_boundaries(int* to_match, int lb, int ub){
    if(*to_match == INT_MIN)
        *to_match = *to_match + 1;
    *to_match = abs(*to_match);
    *to_match = (*to_match)%(ub - lb + 1) + lb;
}

void get_matrix(int ***matrix, int height, int width, FILE* r_stream){
    for(int y = 0; y < height; y++){
        (*matrix)[y] = calloc(width, sizeof(int));
        for(int x = 0; x < width; x++){
            fread(&((*matrix)[y][x]), sizeof(int), 1, r_stream);
            match_boundaries(&((*matrix)[y][x]), -100, 100);
        }
    }
}

void write_to_file(int **matrix, int height, int width, int i, char* type){
    int len = snprintf(NULL, 0, "m%d%s.txt", i, type) + 1;
    char path[len];
    snprintf(path, len, "m%d%s.txt", i, type);
    FILE* f = fopen(path, "w");

    for(int y = 0; y < height; y++){
        for(int x = 0; x < width - 1; x++)
            fprintf(f, "%d ", matrix[y][x]);
        if(y == height)
            fprintf(f, "%d", matrix[y][width-1]);
        else
            fprintf(f, "%d\n", matrix[y][width-1]);
    }
    fclose(f);
}

int main(int argc, char** argv) {
    printf("gen\n");
    if (argc != 4)
        error("Invalid num of arguments");

    int n = atoi(argv[1]);
    int min_matrix_len = atoi(argv[2]);
    int max_matrix_len = atoi(argv[3]);

    r_stream = fopen("/dev/random", "r");
    if (r_stream == NULL)
        error("Coudn't open file rand");

    for(int i = 0;i < n;i++){
        int a_height, a_width, b_width;
        fread(&a_height, sizeof(int), 1, r_stream);
        fread(&a_width, sizeof(int), 1, r_stream);
        fread(&b_width, sizeof(int), 1, r_stream);
        match_boundaries(&a_height, min_matrix_len, max_matrix_len);
        match_boundaries(&a_width, min_matrix_len, max_matrix_len);
        match_boundaries(&b_width, min_matrix_len, max_matrix_len);
        int **a_matrix = calloc(a_height, sizeof(int*));
        int **b_matrix = calloc(a_width, sizeof(int*));
        get_matrix(&a_matrix, a_height, a_width, r_stream);
        get_matrix(&b_matrix, a_width, b_width, r_stream);
        write_to_file(a_matrix, a_height, a_width, i, "a");
        write_to_file(b_matrix, a_width, b_width, i, "b");
    }
    fclose(r_stream);
    return 0;
}