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
    main_table* mt = create_table(atoi(size));            
    printf("Create table size %s:\n", size);
    fprintf(f, "Create table size %s:\n", size);
    return mt;
}

main_table* time_get_main(FILE* f, char* tmp_file) {
    main_table* mt = get_main_table(tmp_file);
    printf("Get main table from tmp file %s\n:", tmp_file);
    fprintf(f, "Get main table from tmp file %s\n:", tmp_file);
    return mt;
}

void time_compare(FILE* f, char* files_seq) {
    compare_pairs(files_seq);
    printf("Comparing pairs %s:\n", files_seq);
    fprintf(f, "Comparing pairs %s:\n", files_seq);
}

void time_del_block(FILE* f, char* block_num, main_table* mt) {
    if (!is_number(block_num))
        error("Didin't passed number into delete_block");
    delete_block(mt, atoi(block_num));
    printf("Deleting block %s:\n", block_num);
    fprintf(f, "Deleting block %s:\n", block_num);
}

void time_del_segment(FILE* f, char* block_num, char* segment_num, main_table* mt) {
    if (!is_number(block_num) || !is_number(segment_num))
        error("Didin't passed number into remove_segment");
    delete_segment(mt, atoi(block_num), atoi(segment_num));
    printf("Deleting segment %s:", segment_num);
    printf(" from block %s:\n", block_num);
    fprintf(f, "Deleting segment %s:", segment_num);
    fprintf(f, " from block %s:\n", block_num);
}

void time_diff(FILE* f, char* afile, char* bfile, char* tmp_file) {
    diff_to_tmp_file(afile, bfile, tmp_file);
    printf("Saving diff to tmp file %s\n:", tmp_file);
    fprintf(f, "Saving diff to tmp file %s\n:", tmp_file);
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

char* get_file_seq(int file_pairs_num) {
    char* files = calloc(1, sizeof(char));
    for (int i = 0; i < file_pairs_num * 2; i += 2) {

    }
}

int main(int argc, char** argv) {
    int file_path_len = strlen(folder_path) + strlen(format) + 1;  // +1 because of 1 letter name
    main_table* mt;
    FILE* f = fopen("raport2.txt", "a");

    char a = 'a';
    char* tmp = "ala";
    char b[4];
    b[0] = 'a';
    b[1] = 'b';
    b[2] = 'c';
    char c[2];
    c[0] = 'h';
    printf("%zu\n", strlen(c));
    printf("%zu\n", strlen(b));
    printf("%zu\n", strlen(tmp));
    printf("%zu\n", strlen(&a));

    char* pair = get_pair(0, file_path_len);
    printf("%s\n", pair);

    int file_pairs_num;
    if (strcmp(argv[1], "small") == 0) {
        file_pairs_num = 3;
    } else if (strcmp(argv[1], "medium")) {
        file_pairs_num = 6;
    } else {
        file_pairs_num = 13;
    }

    return 0;
}