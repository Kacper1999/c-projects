#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "code_os2.h"


int main(int argc, char** argv) {
    if (argc < GEN_ARGS_NUM) 
        error("Not enough arguments");
    
    char* operation = argv[1];
    if (strcmp(operation, "sort") == 0) 
        sort(argc, argv);
    else if (strcmp(operation, "copy") == 0)
        copy(argc, argv);
    else if (strcmp(operation, "generate") == 0)
        generate(argc, argv);
    else 
        error("Coudn't process arguments");
    return 0;
}