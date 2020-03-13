#include "lib_so1.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/times.h>
#include <time.h>
#include <ctype.h>

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

int main(int argc, char* argv[]) {
    main_table* mt;
    FILE* f = fopen("raport2.txt", "a");
    if (f == NULL) 
        error("couldn't open raport2.txt file\n");
    if(argc < 2)
        error("Not enough arguments\n");


    for(int i = 1; i < argc; i++) {
        struct tms* start = calloc(1, sizeof(struct tms));
        struct tms* end = calloc(1, sizeof(struct tms));
        long cstart;
        long cend;
        times(start);
        cstart = clock();
        // I know to get the mose precise measurment we would need to 
        // time it right before the function and right after
        // but it would take alot more code and I dont think its worth it 
        if (strcmp(argv[i], "get_main_table") == 0) {
            mt = time_get_main(f, argv[++i]);
        } else if (strcmp(argv[i], "create_table") == 0) {
            mt = time_create(f, argv[++i]);
        } else if (strcmp(argv[i], "delete_block") == 0) {
            time_del_block(f, argv[++i], mt);
        } else if (strcmp(argv[i], "delete_segment") == 0) {
            time_del_segment(f, argv[i + 1], argv[i + 2], mt);
            i += 2;
        } else if (strcmp(argv[i], "diff_to_tmp_file") == 0) {
            time_diff(f, argv[i + 1], argv[i + 2], argv[i + 3]);
            i += 3;
        } else if (strcmp(argv[i], "compare_pairs") == 0) {
            time_compare(f, argv[++i]);
        } else {
            error("Didn't recognized function");
        }
    
        cend = clock();
        times(end);
        write_result(f, cstart, cend, start, end);
    }
    fclose(f);
    return 0;
}