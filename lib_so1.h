#pragma once

typedef struct file_pair {} file_pair;
typedef struct file_seq {} file_seq;
typedef struct block {} block;
typedef struct main_table {} main_table;

void diff_to_tmp_file(char* afile, char* bfile, char* tmp_file);
block* get_block(char* tmp_file);
main_table* get_main_table(char* files);
void delete_block(main_table* mt, int block_num);
int add_block(main_table* mt, char* tmp_file);
void delete_segment(main_table* mt, int block_num, int segment_num);
int segments_num(main_table mt, int block_num);
main_table* create_table(int blocks_num);
void compare_pairs(char* files);
void concatenate(char** original, char* to_con);
void print_main_table(main_table* mt);