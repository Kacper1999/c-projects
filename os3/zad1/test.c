#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

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
    printf("l = %d, r = %d, PID = %d\n", l, r, getpid());
    for (int i = 0; i < c->height; i++) {
        for (int j = l; j < r; j++) {
            int cell = 0;
            for (int k = 0; k < a->width; k++)
                cell += a->mat[i][k] * b->mat[k][j];
            printf("cell = %d, i = %d, j = %d\n", cell, i, j);
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

void print_childern(pid_t *childern, int n) {
    for (int i = 0; i < n; i++) 
        printf("%d ", childern[i]);
    printf("\n");
}

int main()
{
    // matrix *a = matrix_from_file("ma.txt");
    // printf("\n");
    // print_matrix(a);

    // matrix *b = matrix_from_file("mb.txt");
    // printf("\n");
    // print_matrix(b);

    // matrix *c = matrix_init(a->height, b->width);
    // pll_matrix_mlp(a, b, c, 2);
    // print_matrix(c);

    int n = 5;
    char *fp = "inst.txt";
    FILE* f = fopen(fp, "r");
    
    pid_t root = getpid();
    pid_t *children = calloc(n, sizeof(pid_t));

    for (int i = 0; i < n; i++) {
        pid_t child;
        if ((child = fork()) == 0) {
            break;
        } else {
            children[i] = child;
        }
    }
    if (getpid() == root)
        print_childern(children, n);

    // char c;
    // while ((c = fgetc(f)) != EOF) {
    //     for (int i = 0; i < n; i++) {
    //         if (getpid() != root) {
    //             printf("PID = %d, %c ", getpid(), c);
    //         } else {
    //             fork();
    //         }
    //     }
    // }

    return 0;
}