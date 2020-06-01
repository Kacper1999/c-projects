#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>

#define PRINTY_BOI "PRINTY_BOI\n"

void lib_swap(char* file_path, int records_len, int l, int r) {
    FILE* file = fopen(file_path, "r+");
    fseek(file, l * records_len, SEEK_SET);
    
    char* l_record = calloc(records_len + 1, sizeof(char));
    fread(l_record, sizeof(char), records_len, file);
    l_record[records_len] = '\0';
    
    fseek(file, r * records_len, SEEK_SET);

    char* r_record = calloc(records_len + 1, sizeof(char));
    fread(r_record, sizeof(char), records_len, file);
    r_record[records_len] = '\0';
    
    fseek(file, l * records_len, SEEK_SET);
    fwrite(r_record, sizeof(char), records_len, file);

    fseek(file, r * records_len, SEEK_SET);
    fwrite(l_record, sizeof(char), records_len, file);

    fclose(file);
}

int lib_partition(char* file_path, int records_len, int low, int high) {
    FILE* file = fopen(file_path, "r");
    fseek(file, low * records_len, SEEK_SET);
    char* pivot = calloc(records_len + 1, sizeof(char));
    char* current = calloc(records_len + 1, sizeof(char));

    fread(pivot, sizeof(char), records_len, file);
    pivot[records_len] = '\0';
    printf("low = %d\n", low);
    printf("high = %d\n", high);

    int l = low;
    for (int r = low + 1; r < high; r++) {
        fseek(file, r * records_len, SEEK_SET);
        fread(current, sizeof(char), records_len, file);
        current[records_len] = '\0';
        printf("current = %s", current);
        printf("pivot = %s", pivot);
        printf("Compare = %d\n", strcmp(pivot, current));

        if (strcmp(pivot, current) > 0) {
            fclose(file);
            lib_swap(file_path, records_len, ++l, r);   // ++l instead of l++ because our pivot is the first element
            file = fopen(file_path, "r");
        }
    }
    fclose(file);
    lib_swap(file_path, records_len, l, low);
    
    return l;
}

void sys_swap(char* file_path, int records_len, int l, int r) {
    int file = open(file_path, O_RDONLY);
    lseek(file, l * records_len, SEEK_SET);
    
    char* l_record = calloc(records_len + 1, sizeof(char));
    read(file, l_record, records_len);
    l_record[records_len] = '\0';
    
    lseek(file, r * records_len, SEEK_SET);

    char* r_record = calloc(records_len + 1, sizeof(char));
    read(file, r_record, records_len);
    r_record[records_len] = '\0';
    
    close(file);

    file = open(file_path, O_WRONLY);
    
    lseek(file, l * records_len, SEEK_SET);
    write(file, r_record, records_len);

    lseek(file, r * records_len, SEEK_SET);
    write(file, l_record, records_len);

    close(file);
}

int sys_partition(char* file_path, int records_len, int low, int high) {
    int file = open(file_path, O_RDONLY);
    lseek(file, low * records_len, SEEK_SET);
    char* pivot = calloc(records_len + 1, sizeof(char));
    char* current = calloc(records_len + 1, sizeof(char));

    read(file, pivot, records_len);
    pivot[records_len] = '\0';

    int l = low;
    for (int r = low + 1; r < high; r++) {
        lseek(file, r * records_len, SEEK_SET);
        read(file, current, records_len);
        current[records_len] = '\0';
        // printf("current = %s", current);
        // printf("pivot = %s", pivot);
        // printf("Compare = %d\n", strcmp(pivot, current));

        if (strcmp(pivot, current) > 0) {
            close(file);
            sys_swap(file_path, records_len, ++l, r);   // ++l instead of l++ because our pivot is the first element
            file = open(file_path, O_RDONLY);
        }
    }
    close(file);
    sys_swap(file_path, records_len, l, low);
    
    return l;
}

void _sort(char* file_path, int records_len, char* mode, int low, int high) {
    if (low < high) {
        int pivot;
        if (strcmp(mode, "sys") == 0)
            pivot = sys_partition(file_path, records_len, low, high);
        else
            pivot = lib_partition(file_path, records_len, low, high);
        _sort(file_path, records_len, mode, low, pivot);
        _sort(file_path, records_len, mode, pivot + 1, high);
    }
}

void sort(char* file_path, int records_num, int records_len, char* mode) {
    _sort(file_path, records_len, mode, 0, records_num);
}


int main() {
    char* tmp_file = "tmp.txt";
    int tmp_record_len = 5;
    int tmp_records_num = 10;

    char* a_file = "a.txt";
    char* b_file = "b.txt";
    int f = open(tmp_file, O_RDONLY);

    int* order = calloc(tmp_records_num, sizeof(int));
    for (int i = 0; i < tmp_records_num; i++)
        order[i] = i;
    
    sort(tmp_file, tmp_records_num, tmp_record_len, "lib");

    return 0;
}