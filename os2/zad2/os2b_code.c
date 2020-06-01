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

void error(char *message)
{
    printf("%s\n", message);
    exit(1);
}

char *get_file_type(int st_mode)
{
    if (S_ISREG(st_mode))
        return "file";
    if (S_ISDIR(st_mode))
        return "dir";
    if (S_ISLNK(st_mode))
        return "slink";
    if (S_ISCHR(st_mode))
        return "char dev";
    if (S_ISBLK(st_mode))
        return "block dev";
    if (S_ISSOCK(st_mode))
        return "sock";
    if (S_ISFIFO(st_mode))
        return "fifo";

    return "error";
}

void print_stat(const char *path, const struct stat *file_stat)
{
    char buffer[PATH_MAX];
    const char date_format[] = "%Y-%m-%d %H:%M:%S";
    printf(" %ld \t", file_stat->st_nlink);
    printf(" %s \t", get_file_type(file_stat->st_mode));
    printf(" %ld\t", file_stat->st_size);
    strftime(buffer, PATH_MAX, date_format, localtime(&file_stat->st_atime));
    printf(" %s\t", buffer);
    strftime(buffer, PATH_MAX, date_format, localtime(&file_stat->st_mtime));
    printf(" %s\t", buffer);
    printf(" %s \t\n", path);
}
void print_names()
{
    // I know triple \t looks terrible but it's something
    printf(" links\t type\t size\t atime\t\t\t mtime\t\t\t path\t\n");
}
