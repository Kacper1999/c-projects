#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>

#define GEN_ARGS_NUM 5
#define SORT_ARGS_NUM 6
#define COPY_ARGS_NUM 7

void error(char *message)
{
    printf("%s\n", message);
    exit(1);
}

void print_records(char **records, int records_num);

typedef struct file_struct
{
    int records_num;
    int records_len;
    char **records;
} file_struct;
void print_file_struct(file_struct *fs)
{
    printf("File struct with %d records, length %d each.\n", fs->records_num, fs->records_len);
    print_records(fs->records, fs->records_num);
}

char **get_records(int records_num, int records_len)
{
    srand(time(NULL));

    char **records = calloc(records_num, sizeof(char *));
    for (int i = 0; i < records_num; i++)
    {
        records[i] = calloc(records_len + 1, sizeof(char));

        // -1 because I will add \n to make them easier to see
        // I don't know if that's against the rules
        for (int j = 0; j < records_len - 1; j++)
        {
            records[i][j] = 'A' + rand() % 26;
        }
        records[i][records_len - 1] = '\n';
        records[i][records_len] = '\0';
    }
    return records;
}

file_struct *get_file_struct(int records_num, int records_len)
{
    file_struct *fs = calloc(1, sizeof(file_struct));
    fs->records_num = records_num;
    fs->records_len = records_len;
    fs->records = get_records(records_num, records_len);

    return fs;
}

void print_records(char **records, int records_num)
{
    for (int i = 0; i < records_num; i++)
        printf("Record %d: %s", i, records[i]);
}

void write_records(char *file_path, file_struct *fs)
{
    FILE *f = fopen(file_path, "w");
    for (int i = 0; i < fs->records_num; i++)
        fprintf(f, "%s", fs->records[i]);

    fclose(f);
}

file_struct *read_file_struct(char *file_path, int records_num)
{
    FILE *f = fopen(file_path, "r");
    char *line = NULL;
    size_t len = 0;

    int records_len;
    char **records = calloc(records_num, sizeof(char *));
    for (int i = 0; i < records_num; i++)
    {
        records_len = getline(&line, &len, f);
        char *tmp = calloc(records_len + 1, sizeof(char));
        strcpy(tmp, line);
        records[i] = tmp;
    }
    file_struct *fs = calloc(1, sizeof(file_struct));
    fs->records_len = records_len;
    fs->records_num = records_num;
    fs->records = records;
    fclose(f);
    return fs;
}

void generate(int argc, char **argv)
{
    if (argc != GEN_ARGS_NUM)
        error("Invalid number of arguments");
    char *file_path = argv[2];
    int records_num = atoi(argv[3]);
    int records_len = atoi(argv[4]);
    write_records(file_path, get_file_struct(records_num, records_len));
}

void lib_swap(char *file_path, int records_len, int l, int r)
{
    FILE *file = fopen(file_path, "r+");
    fseek(file, l * records_len, SEEK_SET);

    char *l_record = calloc(records_len + 1, sizeof(char));
    fread(l_record, sizeof(char), records_len, file);
    l_record[records_len] = '\0';

    fseek(file, r * records_len, SEEK_SET);

    char *r_record = calloc(records_len + 1, sizeof(char));
    fread(r_record, sizeof(char), records_len, file);
    r_record[records_len] = '\0';

    fseek(file, l * records_len, SEEK_SET);
    fwrite(r_record, sizeof(char), records_len, file);

    fseek(file, r * records_len, SEEK_SET);
    fwrite(l_record, sizeof(char), records_len, file);

    fclose(file);
}

int lib_partition(char *file_path, int records_len, int low, int high)
{
    FILE *file = fopen(file_path, "r");
    fseek(file, low * records_len, SEEK_SET);
    char *pivot = calloc(records_len + 1, sizeof(char));
    char *current = calloc(records_len + 1, sizeof(char));

    fread(pivot, sizeof(char), records_len, file);
    pivot[records_len] = '\0';

    int l = low;
    for (int r = low + 1; r < high; r++)
    {
        fseek(file, r * records_len, SEEK_SET);
        fread(current, sizeof(char), records_len, file);
        current[records_len] = '\0';

        if (strcmp(pivot, current) > 0)
        {
            fclose(file);
            lib_swap(file_path, records_len, ++l, r); // ++l instead of l++ because our pivot is the first element
            file = fopen(file_path, "r");
        }
    }
    fclose(file);
    lib_swap(file_path, records_len, l, low);

    return l;
}

void sys_swap(char *file_path, int records_len, int l, int r)
{
    int file = open(file_path, O_RDONLY);
    lseek(file, l * records_len, SEEK_SET);

    char *l_record = calloc(records_len + 1, sizeof(char));
    read(file, l_record, records_len);
    l_record[records_len] = '\0';

    lseek(file, r * records_len, SEEK_SET);

    char *r_record = calloc(records_len + 1, sizeof(char));
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

int sys_partition(char *file_path, int records_len, int low, int high)
{
    int file = open(file_path, O_RDONLY);
    lseek(file, low * records_len, SEEK_SET);
    char *pivot = calloc(records_len + 1, sizeof(char));
    char *current = calloc(records_len + 1, sizeof(char));

    read(file, pivot, records_len);
    pivot[records_len] = '\0';

    int l = low;
    for (int r = low + 1; r < high; r++)
    {
        lseek(file, r * records_len, SEEK_SET);
        read(file, current, records_len);
        current[records_len] = '\0';

        if (strcmp(pivot, current) > 0)
        {
            close(file);
            sys_swap(file_path, records_len, ++l, r); // ++l instead of l++ because our pivot is the first element
            file = open(file_path, O_RDONLY);
        }
    }
    close(file);
    sys_swap(file_path, records_len, l, low);

    return l;
}

void _sort(char *file_path, int records_len, char *mode, int low, int high)
{
    if (low < high)
    {
        int pivot;
        if (strcmp(mode, "sys") == 0)
            pivot = sys_partition(file_path, records_len, low, high);
        else
            pivot = lib_partition(file_path, records_len, low, high);
        _sort(file_path, records_len, mode, low, pivot);
        _sort(file_path, records_len, mode, pivot + 1, high);
    }
}

void sort(int argc, char **argv)
{
    if (argc != SORT_ARGS_NUM)
        error("Invalid number of arguments");

    _sort(argv[2], atoi(argv[4]), argv[5], 0, atoi(argv[3]));
}

void sys_copy(char *file1, char *file2, int records_num, int records_len)
{
    int f1 = open(file1, O_RDONLY);
    int f2 = open(file2, O_WRONLY | O_APPEND);

    char *buffer = calloc(records_len, sizeof(char));
    for (int i = 0; i < records_num; i++)
    {
        read(f1, buffer, records_len);
        write(f2, buffer, records_len);
    }

    close(f1);
    close(f2);
}

void c_copy(char *file1, char *file2, int records_num, int records_len)
{
    FILE *f1 = fopen(file1, "r");
    FILE *f2 = fopen(file2, "a");

    char *buffer = calloc(records_len, sizeof(char));
    for (int i = 0; i < records_num; i++)
    {
        fread(buffer, sizeof(char), records_len, f1);
        fwrite(buffer, sizeof(char), records_len, f2);
    }

    fclose(f1);
    fclose(f2);
}

void copy(int argc, char **argv)
{
    if (argc != COPY_ARGS_NUM)
        error("Invalid number of arguments");

    if (argv[COPY_ARGS_NUM - 1] == "sys")
        sys_copy(argv[3], argv[4], atoi(argv[5]), atoi(argv[6]));
    else
        c_copy(argv[3], argv[4], atoi(argv[5]), atoi(argv[6]));
}
