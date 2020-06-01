#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include<sys/wait.h> 

#define PRINTY_BOI "printy boi\n"

typedef struct matrix {
    int height;
    int width;
    int **mat;
} matrix;

matrix *matrix_init(int heigth, int width) {
    matrix *m = calloc(1, sizeof(matrix *));
    m->width = width;
    m->height = heigth;
    m->mat = calloc(heigth, sizeof(int *));
    for (int i = 0; i < heigth; i++)
        m->mat[i] = calloc(width, sizeof(int));
    return m;
}

void print_matrix(matrix *m) {
    for (int i = 0; i < m->height; i++) {
        for (int j = 0; j < m->width; j++)
            printf("%d\t", m->mat[i][j]);
        printf("\n");
    }
    printf("\n");
}

matrix *get_matrix(int height, int width) {
    matrix *m = calloc(1, sizeof(matrix));
    m->height = height;
    m->width = width;
    m->mat = calloc(height, sizeof(int *));

    for (int i = 0; i < height; i++) {
        m->mat[i] = calloc(width, sizeof(int));
        for (int j = 0; j < width; j++)
            scanf("%d", &m->mat[i][j]);
    }
    return m;
}

matrix *matrix_from_file(char *f_path) {
    FILE *f = fopen(f_path, "r");
    char *line = NULL;
    size_t len = 0;
    getline(&line, &len, f);

    char *tmp = NULL;
    char *dim_sep = "x";
    tmp = strtok(line, dim_sep);
    int height = atoi(tmp);

    tmp = strtok(NULL, dim_sep);
    int width = atoi(tmp);

    matrix *m = calloc(1, sizeof(matrix));
    m->width = width;
    m->height = height;
    m->mat = calloc(height, sizeof(int *));

    char *value_sep = "\t";
    for (int i = 0; i < height; i++) {
        m->mat[i] = calloc(width, sizeof(int));
        getline(&line, &len, f);
        tmp = strtok(line, value_sep);
        m->mat[i][0] = atoi(tmp);
        for (int j = 1; j < width; j++) {
            tmp = strtok(NULL, value_sep);
            m->mat[i][j] = atoi(tmp);
        }
    }
    return m;
}

void matrix_mlp(matrix *a, matrix *b, matrix *c, int l, int r) {
    // printf("l = %d, r = %d, PID = %d\n", l, r, getpid());
    for (int i = 0; i < c->height; i++) {
        for (int j = l; j < r; j++) {
            int cell = 0;
            for (int k = 0; k < a->width; k++)
                cell += a->mat[i][k] * b->mat[k][j];
            // printf("cell = %d, i = %d, j = %d\n", cell, i, j);
            c->mat[i][j] = cell;
        }
    }
}

matrix *pll_matrix_mlp(matrix *a, matrix *b, matrix *c, int n) {
    int rows_per_proc = b->width / n;
    if (rows_per_proc == 0)
        rows_per_proc = 1;

    // b->width - rows_per_proc because we execute last instruction after the loop
    int i = 0;
    for (; i < b->width - rows_per_proc; i += rows_per_proc) {
        if (fork() == 0) {
            matrix_mlp(a, b, c, i, i + rows_per_proc);
            exit(0);
        }
    }
    matrix_mlp(a, b, c, i, b->width);
}

// void prepare_file(char* file_path, int height, int width) {
//     FILE *f = fopen(file_path,)
// }

void child_labor(matrix *a, matrix *b, matrix *c, char* line) {
    char *sep = "-";
    int l = atoi(strtok(line, sep));
    int r = atoi(strtok(NULL, sep));
    matrix_mlp(a, b, c, l, r);
}

matrix* ex(char* inst_path, int n) {
    FILE *f = fopen(inst_path, "r");
    
    char *line = NULL;
    size_t len = 0;
    getline(&line, &len, f);
    line[strlen(line) - 1] = '\0'; // get rid of the \n char

    char *tmp = NULL;
    char *sep = " ";
    char *a_file = strtok(line, sep);
    char *b_file = strtok(NULL, sep);
    char *c_file = strtok(NULL, sep);
    
    printf("%s\n", a_file);
    printf("%s\n", b_file);
    printf("%s\n", c_file);
    matrix *a = matrix_from_file(a_file);
    matrix *b = matrix_from_file(b_file);
    matrix *c = matrix_init(a->height, b->width);
    // preapare_file(c_file, a_matrix->height, b_matrix->width);

    pid_t root = getpid();
    pid_t *children = calloc(n, sizeof(pid_t));

    while (1) {
        for (int i = 0; i < n; i++) {
            if (getline(&line, &len, f) <= 0)
                break;
            pid_t child;
            if ((child = fork()) == 0) {
                child_labor(a, b, c, line);
                exit(0);
            } else {
                children[i] = child;
            }       
        }
    }

    pid_t wpid;
    while ((wpid = wait(NULL)) > 0); 
    fclose(f);
    return c;
}


int main(int argc, char** argv) {
    matrix* c = ex("inst.txt", 5);
    print_matrix(c);
    return 0;
}