#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>

#define TMP_FILE "tmp.txt"
#define PRINTY_BOI "PRINTY_BOI\n"

void print_records(char** records, int records_num);

typedef struct file_struct {
    int records_num;
    int records_len;
    char** records;
}file_struct;

void print_file_struct(file_struct* fs) {
    printf("File struct with %d records, length %d each.\n", fs->records_num, fs->records_len);
    print_records(fs->records, fs->records_num);
}

char** get_records(int records_num, int records_len) {
    srand(time(NULL));

    char** records = calloc(records_num, sizeof(char*));
    for (int i = 0; i < records_num; i++) {
        records[i] = calloc(records_len + 1, sizeof(char));
        
        // -1 because I will add \n to make them easier to see
        // I don't know if that's against the rules
        for (int j = 0; j < records_len - 1; j++) {
            records[i][j] = 'A' + rand() % 26;
        }
        records[i][records_len - 1] = '\n';
        records[i][records_len] = '\0';
    }
    return records;
}

file_struct* get_file_struct(int records_num, int records_len) {
    file_struct* fs = calloc(1, sizeof(file_struct));
    fs->records_num = records_num;
    fs->records_len = records_len;
    fs->records = get_records(records_num, records_len);

    return fs;
}

void print_records(char** records, int records_num) {
    for (int i = 0; i < records_num; i++) 
        printf("Record %d: %s", i, records[i]);
}

void write_records(file_struct* fs) {
    FILE* f = fopen(TMP_FILE, "w");
    for (int i = 0; i < fs->records_num; i++) 
        fprintf(f, "%s", fs->records[i]);
    
    fclose(f);
}

file_struct* read_file_struct(int records_num) {
    FILE* f = fopen(TMP_FILE, "r");
    char* line = NULL;
    size_t len = 0;

    int records_len;
    char** records = calloc(records_num, sizeof(char*));
    for (int i = 0; i < records_num; i++) {
        records_len = getline(&line, &len, f);
        char* tmp = calloc(records_len + 1, sizeof(char));
        strcpy(tmp, line);
        records[i] = tmp;
    }
    file_struct* fs = calloc(1, sizeof(file_struct));
    fs->records_len = records_len;
    fs->records_num = records_num;
    fs->records =records;
    fclose(f);
    return fs;
}

int main() {
    int records_num = 10;
    int records_len = 5;

    file_struct* fs = get_file_struct(records_num, records_len);

    write_records(fs);
    
    file_struct* fs2 = read_file_struct(records_num);

    return 0;
}