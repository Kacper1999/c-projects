#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

bool concurrent_safe = true;
int interval;
int excess;
int max_gray;
int **img;
int width;
int height;
int *result;
pthread_mutex_t arr_mutex = PTHREAD_MUTEX_INITIALIZER;

int min(int a, int b) {
    return a > b ? b : a;
}

long get_time(struct timeval start, struct timeval end) {
    long time = (end.tv_sec - start.tv_sec) * (int)1e6;
    time += end.tv_usec - start.tv_usec;
    return time;
}

void *sign_func(void *t_num_arg) {
    long t_num = (long)t_num_arg;
    int min_val = t_num * interval + min(excess, t_num);
    int max_val = min_val + interval;
    if (t_num < excess)
        max_val++;

    struct timeval start, end;
    gettimeofday(&start, NULL);
    for (int i = 0; i < height; i++)
        for (int j = 0; j < width; j++)
            if (min_val <= img[i][j] && img[i][j] < max_val) {
                if (concurrent_safe) {
                    pthread_mutex_lock(&arr_mutex);
                    result[img[i][j]]++;
                    pthread_mutex_unlock(&arr_mutex);
                } else {
                    result[img[i][j]]++;
                }
            }
    gettimeofday(&end, NULL); 
    pthread_exit((void *)(get_time(start, end)));
}

void *block_func(void *t_num_arg) {
    long t_num = (long)t_num_arg;
    int min_j = t_num * interval + min(excess, t_num);
    int max_j = min_j + interval;
    if (t_num < excess)
        max_j++;

    struct timeval start, end;
    gettimeofday(&start, NULL);
    for (int i = 0; i < height; i++)
        for (int j = min_j; j < max_j; j++) {
            if (concurrent_safe) {
                pthread_mutex_lock(&arr_mutex);
                result[img[i][j]]++;
                pthread_mutex_unlock(&arr_mutex);
            } else {
                result[img[i][j]]++;
            }
        }
    gettimeofday(&end, NULL);  
    pthread_exit((void *)(get_time(start, end)));
}

void *interleaved_func(void *t_num_arg) {
    long t_num = (long)t_num_arg;

    struct timeval start, end;
    gettimeofday(&start, NULL);
    for (int i = 0; i < height; i++)
        for (int j = t_num; j < width; j += interval) {
            if (concurrent_safe) {
                pthread_mutex_lock(&arr_mutex);
                result[img[i][j]]++;
                pthread_mutex_unlock(&arr_mutex);
            } else {
                result[img[i][j]]++;
            }
        }
    gettimeofday(&end, NULL);
    pthread_exit((void *)(get_time(start, end)));
}


int get_val(FILE *f) {
    int val = 0;
    for (char c = fgetc(f); c != ' '; c = fgetc(f))
        if (c != '\n')
            val = val * 10 + (int)c - '0';
    return val;
}

void init_global_val(char *file_path) {
    FILE *f = fopen(file_path, "r");
    // get width and height
    char *line = NULL;
    size_t n = 0;
    if (getline(&line, &n, f) == -1)
        perror("getline magic number");
    if (getline(&line, &n, f) == -1)
        perror("getline width height");
    if (line[0] == '#')  // theoretically we should check this every time
        if (getline(&line, &n, f) == -1)
            perror("getline# width height");
    width = atoi(strtok(line, " "));
    height = atoi(strtok(NULL, " "));
    img = malloc(height * sizeof(int*));
    for (int i = 0; i < height; i++)
        img[i] = malloc(width * sizeof(int));

    // get max gray value
    if (getline(&line, &n, f) == -1)
        perror("getline max gray");
    max_gray = atoi(line) + 1;
    result = calloc(max_gray, sizeof(int));
    free(line);

    // get image as 2d arr
    for (int i = 0; i < height; i++) 
        for (int j = 0; j < width; j++) 
            img[i][j] = get_val(f);
    fclose(f);
}

int main(int argc, char **argv) {
    char *input_file_path = argv[1];
    char *mode = argv[2];
    int threads_num = atoi(argv[3]);
    char *output_file_path = argv[4];
    if (argc == 6)
        concurrent_safe = false;

    init_global_val(input_file_path);
    void *func;
    
    if (strcmp(mode, "sign") == 0) {
        interval = max_gray / threads_num;
        excess = max_gray % threads_num;
        func = sign_func;
    } else if (strcmp(mode, "block") == 0) {
        interval = width / threads_num;
        excess = width % threads_num;  
        func = block_func;
    } else {
        interval = threads_num;
        excess = 0;
        func = interleaved_func;
    }

    struct timeval start, end;
    gettimeofday(&start, NULL);

    pthread_t *threads = malloc(sizeof(pthread_t) * threads_num);  
    for (long i = 0; i < threads_num; i++) 
        pthread_create(&threads[i], NULL, func, (void *)i);

    for (int i = 0; i < threads_num; i++) {
        long t_time;
        pthread_join(threads[i], (void *)(&t_time));
        printf("thread: %d, time: %ld ms\n", i, t_time);
    }

    gettimeofday(&end, NULL);
    long whole_ticks = get_time(start, end);
    printf("\nwhole time: %ld ms\n\n", whole_ticks);

    if (strcmp(output_file_path, "0") != 0) {
        FILE *of = fopen(output_file_path, "w");
        for (int i = 0; i < max_gray; i++)
            fprintf(of, "%d ", result[i]);
        fclose(of);
    }

    return 0;
}