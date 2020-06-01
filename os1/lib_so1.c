#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct file_pair {
    char* first_f;
    char* second_f;
} file_pair;
void print_file_pair(file_pair fp) {
    printf("%s\n", fp.first_f);
    printf("%s\n", fp.second_f);
}

typedef struct file_seq {
    int pairs_num;
    file_pair* file_pairs;
} file_seq;
void print_file_seq(file_seq fs) {
    for (int i = 0; i < fs.pairs_num; i++) {
        printf("%s", "file pair num: ");
        printf("%d\n", i);
        print_file_pair(fs.file_pairs[i]);
        printf("%s\n", "");
    }
}

typedef struct block {
    int segments_num;
    char** segments;
} block;
void print_block(block *b) {
    for (int j = 0; j < b->segments_num; j++) {
        printf("%s", "edit num: ");
        printf("%d\n", j);
        if (b->segments[j] != NULL) {
            printf("%s", b->segments[j]);
        } else {
            printf("NULL\n");
        }
    }
    printf("%c", '\n');
}

typedef struct main_table {
    int blocks_num;
    block** blocks;
} main_table;
void print_main_table(main_table *mt) {
    for (int i = 0; i < mt->blocks_num; i++) {
        block* curr_block = mt->blocks[i];
        printf("%s", "block num: ");
        printf("%d\n", i);
        if (curr_block != NULL) {
            printf("%c", '\n');
            print_block(curr_block);
            printf("%c", '\n');    
        } else {
            printf("NULL\n");
        }
    }

}


void concatenate(char** original, char* to_con) {
    int output_len = strlen(*original) + strlen(to_con);
    char* tmp = calloc(output_len + 1, sizeof(char));

    strcpy(tmp, *original);
    strcat(tmp, to_con);
    tmp[output_len] = '\0';
    *original = tmp;
}

char* str_slice(char* original, int start, int end) {
    char* slice = calloc(end - start + 1, sizeof(char));
    for (int i = 0; i < end - start ; i++) 
        slice[i] = original[start + i];
    return slice;
}

int sep_num_string(char* text, char sep) {
    int result = 0;
    for (int i = 0; i < strlen(text); i++) 
        if (text[i] == sep)
            result++;
    return result;
}

block str_split(char* text, char separator) {
    block b;
    int segments_num = sep_num_string(text, separator) + 1;
    // +1 because we will include the end of the string
    // exmple str_split("a b", ' ') -> sep_num = 1 segments_num = 2
    b.segments_num = segments_num;
    b.segments = calloc(segments_num, sizeof(char*));

    char* segment;
    int prev = 0;
    int segment_num = 0;
    for (int i = 0; i <= strlen(text); i++) {  // <= we need to look for the end of string
        if (text[i] == separator || text[i] == '\0') {
            segment = str_slice(text, prev, i);
            prev = i + 1;  // +1 because we dont want separator
            b.segments[segment_num++] = segment;
        }
    }
    return b;
}

char* line_from_file(FILE* f_ptr) {
    char* line = calloc(1, sizeof(char));  // we need to allocate something for concatanate to work
    int buffer_len = 255;
    char* buffer = calloc(buffer_len, sizeof(char));
    while(fgets(buffer, buffer_len, f_ptr) != NULL) {
        concatenate(&line, buffer);
        if (buffer[strlen(buffer) - 1] == '\n')
            break;
    }
    return line;
}

char* get_line(char* original, int start) {
    int i = start;
    char* line;
    while (original[i] != '\n' || i != strlen(original))
        i++;
    line = str_slice(original, start, i);
    return line;
}

// checks if line is a separator of the diff command output
int is_sep(char* line) {
    int l = strlen(line);
    // check if line is at least 3 chars long, starts and ends with a digit
    // (-2 because last character of line is always newline)
    if (l >= 3 && isdigit(line[0]) && isdigit(line[l - 2]))
        return 1;
    return 0; 
}

// returns number of separators (= block.edits_num)
int sep_num_file(char* tmp_file) {
    FILE* f_ptr = fopen(tmp_file, "r");
    char* line;
    int result = 0;
    while (!feof(f_ptr)) {
        line = line_from_file(f_ptr);
        if (is_sep(line)) {
            result++;
        }
    }
    return result;
}


void diff_to_tmp_file(char* afile, char* bfile, char* tmp_file) {
    char* cmd = "diff ";
    concatenate(&cmd, afile);
    concatenate(&cmd, " ");
    concatenate(&cmd, bfile);
    concatenate(&cmd, " > ");
    concatenate(&cmd, tmp_file);
    system(cmd);
}

file_seq split_files(char* files) {
    // pairs of files are given in a format: "a.txt:b.txt c.txt:d.txt"
    block connected_file_pairs = str_split(files, ' ');

    int pairs_num = connected_file_pairs.segments_num;
    file_pair* file_pairs = calloc(pairs_num, sizeof(file_pair));
    for (int i = 0; i < pairs_num; i++) {
        block tmp = str_split(connected_file_pairs.segments[i], ':');
        file_pairs[i].first_f = tmp.segments[0];
        file_pairs[i].second_f = tmp.segments[1];
    }

    file_seq result;
    result.pairs_num = pairs_num;
    result.file_pairs = file_pairs;
    return result;
}

block* get_block(char* tmp_file) {
    block* b = calloc(1, sizeof(block));
    int edits_num = sep_num_file(tmp_file);
    b->segments_num = edits_num;
    b->segments = calloc(edits_num, sizeof(char*));

    FILE* f_ptr = fopen(tmp_file, "r");
    char* line = line_from_file(f_ptr);
    char* edit_text = calloc(1, sizeof(char));  // we need to allocate to concatane
    int i = 0;
    while (!feof(f_ptr)) {
        if (is_sep(line)) {
            concatenate(&edit_text, line);
            line = line_from_file(f_ptr);
            while (!is_sep(line) && !feof(f_ptr)) {
                concatenate(&edit_text, line);
                line = line_from_file(f_ptr);
            }

        } else {
            printf("%s", line);
            printf("%s\n", "separator not found");
            exit(2);
        }
        b->segments[i++] = edit_text;
        edit_text = calloc(1, sizeof(char));
    }
    return b;
}

main_table* create_table(int blocks_num) {
    main_table* mt = calloc(1, sizeof(main_table*));
    mt->blocks_num = blocks_num;
    mt->blocks = calloc(blocks_num, sizeof(block*));
    return mt;
}

void compare_pairs(char* files) {  //  int show_diffs
    file_seq fs = split_files(files);
    char* cmd;
    for (int i = 0; i < fs.pairs_num; i++) {
        cmd = "diff ";
        // if (show_diffs == 0) {
        // } else {
        //     cmd = "echo diff ";
        // }
        concatenate(&cmd, fs.file_pairs[i].first_f);
        concatenate(&cmd, " ");
        concatenate(&cmd, fs.file_pairs[i].second_f);
        system(cmd);
    }
}

main_table* get_main_table(char* files) {
    file_seq fs = split_files(files);
    main_table* mt = calloc(1, sizeof(main_table));
    mt->blocks_num = fs.pairs_num;
    mt->blocks = calloc(mt->blocks_num, sizeof(block*));
    for (int i = 0; i < fs.pairs_num; i++) {
        char* afile = fs.file_pairs[i].first_f;
        char* bfile = fs.file_pairs[i].second_f;
        char* tmp_file = "tmp.txt";
        diff_to_tmp_file(afile, bfile, tmp_file);
        mt->blocks[i] = get_block(tmp_file);
    }
    return mt;
}

void delete_block(main_table* mt, int block_num) {
    free(mt->blocks[block_num]);
    mt->blocks[block_num] = NULL;
}

int add_block(main_table* mt, char* tmp_file) {
    block* new_block = get_block(tmp_file);
    main_table* tmp = realloc(mt, mt->blocks_num + 1);
    
    if (tmp == NULL) {
        printf("Coudnt reallocate\n");
        exit(3);
    }
    mt = tmp;
    mt->blocks[mt->blocks_num] = new_block;
    mt->blocks_num++;
    return mt->blocks_num;
}

void delete_segment(main_table* mt, int block_num, int segment_num) {
    free(mt->blocks[block_num]->segments[segment_num]);
    mt->blocks[block_num]->segments[segment_num] = NULL;
}

int segments_num(main_table mt, int block_num) {
    return mt.blocks[block_num]->segments_num;
}
