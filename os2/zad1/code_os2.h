#pragma once

#define GEN_ARGS_NUM 5
#define SORT_ARGS_NUM 6
#define COPY_ARGS_NUM 7

void copy(int argc, char** argv);
void sort(int argc, char** argv);
void generate(int argc, char** argv);
void error(char* message);