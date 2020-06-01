#include <time.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <string.h>
#include <linux/limits.h>
#include <unistd.h>
#include <errno.h>
#include <math.h>
#include <time.h>
#include <ftw.h>
#include "os2b.h"

void decide_to_print(const char *path, const struct stat *file_stat, char *time_opt, char *time_opt_arg)
{
    time_t current_time = time(NULL);
    time_t f_time;
    if (strcmp(time_opt, "-mtime") == 0)
        f_time = file_stat->st_mtime;
    else if (strcmp(time_opt, "-atime") == 0)
        f_time = file_stat->st_atime;
    else
        error("Error - invalid option");

    int difference = (int)(difftime(current_time, f_time) / 86400); // 86400 secs in a day
    int show;
    int threshold = abs(atoi(time_opt_arg));
    if (time_opt_arg[0] == '+')
        show = (difference > threshold);
    else if (time_opt_arg[0] == '-')
        show = (difference < threshold);
    else
        show = (difference == threshold);
    if (show)
        print_stat(path, file_stat);
}

void find(char *path, char *time_opt, char *time_opt_arg, int depth)
{
    if (depth == 0 || path == NULL)
        return;

    DIR *dir = opendir(path);
    if (dir == NULL)
        return;
    struct dirent *rdir = readdir(dir);
    struct stat file_stat;
    printf("PID: %d\n", getpid());

    char new_path[PATH_MAX];
    while (rdir != NULL)
    {
        strcpy(new_path, path);
        strcat(new_path, "/");
        strcat(new_path, rdir->d_name);
        lstat(new_path, &file_stat);

        if (!(strcmp(rdir->d_name, ".") == 0 || strcmp(rdir->d_name, "..") == 0))
        {
            decide_to_print(new_path, &file_stat, time_opt, time_opt_arg);
            if (S_ISDIR(file_stat.st_mode))
            {
                if (fork() == 0)
                {
                    find(new_path, time_opt, time_opt_arg, depth - 1);
                    exit(0);
                }
            }
        }
        rdir = readdir(dir);
    }
    closedir(dir);
}

int main(int argc, char **argv)
{

    if (argc < 4)
        error("Invalid number of arguments");

    char path[PATH_MAX];
    realpath(argv[1], path);
    DIR *dir = opendir(path);
    if (dir == NULL)
        error("Couldn't open the directory");
    char *time_opt;
    char *time_opt_arg;
    int depth;
    if (argc == 4) // no maxdepth
    {
        time_opt = argv[2];
        time_opt_arg = argv[3];
        depth = -1;
    }
    else
    {
        if (strcmp(argv[2], "-maxdepth") == 0)
        {
            time_opt = argv[4];
            time_opt_arg = argv[5];
            depth = atoi(argv[3]);
        }
        else
        {
            time_opt = argv[2];
            time_opt_arg = argv[3];
            depth = atoi(argv[5]);
        }
    }
    struct stat file_stat;
    lstat(path, &file_stat); // get file info
    print_names();
    decide_to_print(path, &file_stat, time_opt, time_opt_arg);
    find(path, time_opt, time_opt_arg, depth);

    closedir(dir);
    return 0;
}