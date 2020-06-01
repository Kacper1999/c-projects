#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>

#define PRINTY_BOI "PRINTY_BOI\n"

void sys_copy(char* file1, char* file2, int records_num, int records_len) {
    int f1 = open(file1, O_RDONLY);
    int f2 = open(file2, O_WRONLY | O_APPEND);

    char* buffer = calloc(records_len, sizeof(char));
    for (int i = 0; i < records_num; i++) {
        read(f1, buffer, records_len);
        write(f2, buffer, records_len);
    }

    close(f1);
    close(f2);
}

void c_copy(char* file1, char* file2, int records_num, int records_len) {
    FILE* f1 = fopen(file1, "r");
    FILE* f2 = fopen(file2, "a");

    char* buffer = calloc(records_len, sizeof(char));
    for (int i = 0; i < records_num; i++) {
        fread(buffer, sizeof(char), records_len, f1);
        fwrite(buffer, sizeof(char), records_len, f2);
    }

    fclose(f1);
    fclose(f2);
}

int main() {
    char* tmp_file = "tmp.txt";
    char* a_file = "a.txt";
    char* b_file = "b.txt";

    int buffer_len = 10;
    c_copy(a_file, b_file, 2, 4);
    return 0;
}