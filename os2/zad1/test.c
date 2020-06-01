#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>

#define PRINTY_BOI "PRINTY_BOI\n"

int main() {
    char* tmp_file = "tmp.txt";
    int tmp_record_len = 5;
    int tmp_records_num = 10;

    FILE* f = fopen(tmp_file, "w");
    fseek(f, tmp_record_len, SEEK_SET);

    char* new = calloc(tmp_record_len + 1, sizeof(char));
    new[0] = '0';
    new[1] = '1';
    new[2] = '2';
    new[3] = '3';
    new[4] = '\n';
    new[5] = '\0';

    fputs(new, f);
    fclose(f);

    return 0;
}