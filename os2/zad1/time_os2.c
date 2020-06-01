#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/times.h>
#include <time.h>
#include <unistd.h>
#include "code_os2.h"

#define RESULT_FILE "wyniki.txt"
#define TMP_FILE "tmp.txt"
#define LIB_FILE "lib.txt"
#define SYS_FILE "sys.txt"

double elapsed(clock_t before, clock_t after)
{
    clock_t res = sysconf(_SC_CLK_TCK);
    return (double)(after - before) / res;
}

void write_result(char *command, char *mode, char *records_num, char *records_len, struct tms before, struct tms after)
{
    double s_time = elapsed(before.tms_stime, after.tms_stime);
    double u_time = elapsed(before.tms_utime, after.tms_utime);

    FILE *f = fopen(RESULT_FILE, "a");
    fprintf(f, "%s %s with %s records %s length each:\n", mode, command, records_num, records_len);
    fprintf(f, "\tsys time: %lf\n", s_time);
    fprintf(f, "\tusr time: %lf\n\n", u_time);
    fclose(f);
}

void time_sort(int argc, char **argv)
{
    struct tms before;
    struct tms after;
    times(&before);
    sort(argc, argv);
    times(&after);
    write_result(argv[1], argv[SORT_ARGS_NUM - 1], argv[3], argv[4], before, after);
}

void time_copy(int argc, char **argv)
{
    struct tms before;
    struct tms after;
    times(&before);
    copy(argc, argv);
    times(&after);
    write_result(argv[1], argv[COPY_ARGS_NUM - 1], argv[4], argv[5], before, after);
}

int main(int argc, char **argv)
{
    if (argc < 2)
        error("Not enough arguments");

    int records_num = atoi(argv[1]);
    char *records_len[] = {"1", "4", "512", "1024", "4096", "8192"};
    int diff_records_num = 6;

    for (int i = 0; i < diff_records_num; i++)
    {
        char **gen_argv = calloc(GEN_ARGS_NUM, sizeof(char *));
        gen_argv[1] = "gen";
        gen_argv[2] = TMP_FILE;
        gen_argv[3] = argv[1];
        gen_argv[4] = records_len[i];
        generate(GEN_ARGS_NUM, gen_argv);

        char **copy_argv = calloc(COPY_ARGS_NUM, sizeof(char *));
        copy_argv[1] = "copy";
        copy_argv[2] = TMP_FILE;
        copy_argv[3] = LIB_FILE;
        copy_argv[4] = argv[1];
        copy_argv[5] = records_len[i];
        copy_argv[6] = "lib";
        time_copy(COPY_ARGS_NUM, copy_argv);

        copy_argv[3] = SYS_FILE;
        copy_argv[6] = "sys";
        time_copy(COPY_ARGS_NUM, copy_argv);

        char **sort_argv = calloc(SORT_ARGS_NUM, sizeof(char *));
        sort_argv[1] = "sort";
        sort_argv[2] = LIB_FILE;
        sort_argv[3] = argv[1];
        sort_argv[4] = records_len[i];
        sort_argv[5] = "lib";
        time_sort(SORT_ARGS_NUM, sort_argv);

        sort_argv[2] = SYS_FILE;
        sort_argv[5] = "sys";
        time_sort(SORT_ARGS_NUM, sort_argv);
    }

    return 0;
}