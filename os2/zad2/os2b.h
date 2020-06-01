#pragma once

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <linux/limits.h>

void error(char *message);
void print_stat(const char *path, const struct stat *file_stat);
char *get_file_type(int st_mode);
void print_names();