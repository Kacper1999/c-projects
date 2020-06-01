#define _XOPEN_SOURCE 500
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

int depth;
char *time_opt;
char *time_opt_arg;

static int decide_to_print(const char *path, const struct stat *file_stat, int typeflag, struct FTW *ftwbuf)
{
    if (depth >= 0 && ftwbuf->level > depth)
        return 0;

    time_t current_time = time(NULL);
    time_t f_time; // atime or mtime depanding on the time_opt
    if (strcmp(time_opt, "-mtime") == 0)
        f_time = file_stat->st_mtime;
    else if (strcmp(time_opt, "-atime") == 0)
        f_time = file_stat->st_atime;
    else
        error("Error - invalid option");

    int time_diff = (int)(difftime(current_time, f_time) / 86400); // 86400 secs in a day
    int show;
    int threshold = abs(atoi(time_opt_arg));
    if (time_opt_arg[0] == '+')
        show = (time_diff > threshold);
    else if (time_opt_arg[0] == '-')
        show = (time_diff < threshold);
    else
        show = (time_diff == threshold);
    if (show)
        print_stat(path, file_stat);
    return 0;
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

    print_names();
    nftw(path, decide_to_print, 10, FTW_PHYS);

    closedir(dir);
    return 0;
}
