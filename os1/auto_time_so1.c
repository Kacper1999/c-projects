#include "lib_so1.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/times.h>
#include <time.h>
#include <ctype.h>

#define LETTERS  "abcdefghijklmnopqrstuvwxyz"
#define folder_path "./text_files/"
#define format ".txt"

void error(char* message) {
    printf("%s\n", message);
    exit(1);
}

int is_number(char* text) {
    for (int i = 0; i < strlen(text); i++)
        if (!isdigit(text[i]))
            return 0;
    return 1;
}

double elapsed_time(clock_t start, clock_t end) {
    return ((double)(end - start) / sysconf(_SC_CLK_TCK));
}

void write_result(FILE* f, long cstart, long cend, struct tms* t_start, struct tms* t_end){
    double real_time = (double) (cend - cstart) / CLOCKS_PER_SEC;
    double usr_time = elapsed_time(t_start->tms_utime, t_end->tms_utime);
    double sys_time = elapsed_time(t_start->tms_stime, t_end->tms_stime);

    printf("\tREAL_TIME: %fl\n", real_time);
    printf("\tUSER_TIME: %fl\n", usr_time);
    printf("\tSYSTEM_TIME: %fl\n", sys_time);

    fprintf(f, "\tREAL_TIME: %fl\n", real_time);
    fprintf(f, "\tUSER_TIME: %fl\n", usr_time);
    fprintf(f, "\tSYSTEM_TIME: %fl\n", sys_time);
}

main_table* time_create(FILE* f, char* size) {
    if (!is_number(size))
        error("Didin't passed number into create_table");
    
    struct tms* start = calloc(1, sizeof(struct tms));
    struct tms* end = calloc(1, sizeof(struct tms));
    long cstart;
    long cend;
    times(start);
    cstart = clock();

    main_table* mt = create_table(atoi(size));

    cend = clock();
    times(end);
    printf("Create table size %s:\n", size);
    fprintf(f, "Create table size %s:\n", size);
    write_result(f, cstart, cend, start, end);  
    return mt;
}

main_table* time_get_main(FILE* f, char* tmp_file) {
    struct tms* start = calloc(1, sizeof(struct tms));
    struct tms* end = calloc(1, sizeof(struct tms));
    long cstart;
    long cend;
    times(start);
    cstart = clock();
    
    main_table* mt = get_main_table(tmp_file);

    cend = clock();
    times(end);
    printf("Get main table from tmp file: %s\n", tmp_file);
    fprintf(f, "Get main table from tmp file: %s\n", tmp_file);
    write_result(f, cstart, cend, start, end);
    return mt;
}

void time_compare(FILE* f, char* files_seq) {
    struct tms* start = calloc(1, sizeof(struct tms));
    struct tms* end = calloc(1, sizeof(struct tms));
    long cstart;
    long cend;
    times(start);
    cstart = clock();
    
    compare_pairs(files_seq);
    
    cend = clock();
    times(end);
    printf("Comparing pairs %s:\n", files_seq);
    fprintf(f, "Comparing pairs %s:\n", files_seq);
    write_result(f, cstart, cend, start, end);
}

void time_del_block(FILE* f, char* block_num, main_table* mt) {
    if (!is_number(block_num))
        error("Didin't passed number into delete_block");

    struct tms* start = calloc(1, sizeof(struct tms));
    struct tms* end = calloc(1, sizeof(struct tms));
    long cstart;
    long cend;
    times(start);
    cstart = clock();
    
    delete_block(mt, atoi(block_num));

    cend = clock();
    times(end);
    printf("Deleting block %s:\n", block_num);
    fprintf(f, "Deleting block %s:\n", block_num);
    write_result(f, cstart, cend, start, end);
}

void time_del_segment(FILE* f, char* block_num, char* segment_num, main_table* mt) {
    if (!is_number(block_num) || !is_number(segment_num))
        error("Didin't passed number into remove_segment");
    
    
    struct tms* start = calloc(1, sizeof(struct tms));
    struct tms* end = calloc(1, sizeof(struct tms));
    long cstart;
    long cend;
    times(start);
    cstart = clock();
    
    delete_segment(mt, atoi(block_num), atoi(segment_num));

    cend = clock();
    times(end);
    printf("Deleting segment %s:", segment_num);
    printf(" from block %s:\n", block_num);
    fprintf(f, "Deleting segment %s:", segment_num);
    fprintf(f, " from block %s:\n", block_num);
    write_result(f, cstart, cend, start, end);
}

void time_diff(FILE* f, char* afile, char* bfile, char* tmp_file) {
    struct tms* start = calloc(1, sizeof(struct tms));
    struct tms* end = calloc(1, sizeof(struct tms));
    long cstart;
    long cend;
    times(start);
    cstart = clock();
    
    diff_to_tmp_file(afile, bfile, tmp_file);

    cend = clock();
    times(end);
    printf("Saving diff to tmp file: %s\n", tmp_file);
    fprintf(f, "Saving diff to tmp file: %s\n", tmp_file);
    write_result(f, cstart, cend, start, end);
}

void time_add_del(FILE* f, char* loop) {
    int l = atoi(loop);
    
    struct tms* start = calloc(1, sizeof(struct tms));
    struct tms* end = calloc(1, sizeof(struct tms));
    long cstart;
    long cend;
    times(start);
    cstart = clock();
    
    main_table* mt;
    for (int i = 0; i < l; i++) {
        mt = create_table(1);
        delete_block(mt, 0);
    }

    cend = clock();
    times(end);
    printf("adding and deleting n times: %s\n", loop);
    fprintf(f, "adding and deleting n times: %s\n", loop);
    write_result(f, cstart, cend, start, end);
}

char* get_pair(int i, int file_path_len) {
    char* afile = calloc(file_path_len + 1, sizeof(char));
    concatenate(&afile, folder_path);
    char* bfile = calloc(file_path_len + 1, sizeof(char));
    concatenate(&bfile, folder_path);

    char a_name[2];
    a_name[0] = LETTERS[i];
    a_name[1] = '\0';
    printf("%zu\n", strlen(a_name));
    concatenate(&afile, a_name);

    char b_name[2];
    b_name[0] = LETTERS[i + 1];
    b_name[1] = '\0';
    concatenate(&bfile, b_name);

    concatenate(&afile, format);
    concatenate(&bfile, format);

    char* pair = calloc(1, sizeof(char));
    concatenate(&pair, afile);
    char tmp[2];
    tmp[0] = ':';
    tmp[1] = '\0';
    concatenate(&pair, tmp);
    concatenate(&pair, bfile);
    free(afile);
    free(bfile);
    return pair;
}

char* get_file_seq(int file_pairs_num, int file_path_len) {
    char* output = calloc(1, sizeof(char));
    char* pair = get_pair(0, file_path_len);
    concatenate(&output, pair);
    
    char space[2];
    space[0] = ' ';
    space[1] = '\0';

    for (int i = 2; i < file_pairs_num * 2; i += 2) {
        concatenate(&output, space);
        pair = get_pair(i, file_path_len);
        concatenate(&output, pair);
    }
    return output;
}

void creating_table(FILE* f, char* arg) {
    int file_path_len = strlen(folder_path) + strlen(format) + 1;  // +1 because of 1 letter name
    int file_pairs_num;
    if (strcmp(arg, "small") == 0) {
        file_pairs_num = 3;
    } else if (strcmp(arg, "medium")) {
        file_pairs_num = 6;
    } else {
        file_pairs_num = 13;
    }
    char* files = get_file_seq(file_pairs_num, file_path_len);
    time_get_main(f, files);
}


int main(int argc, char** argv) {
    main_table* mt;
    FILE* f = fopen("raport3a.txt", "a");


    creating_table(f, argv[1]);
    mt = time_create(f, argv[2]);
    time_del_block(f, argv[3], mt);
    time_add_del(f, argv[4]);


    return 0;
}