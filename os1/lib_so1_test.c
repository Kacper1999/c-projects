#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lib_so1.h"


int main() {


    char* test = "./text_files/a.txt:./text_files/b.txt ./text_files/c.txt:./text_files/d.txt ./text_files/e.txt:./text_files/f.txt";
    main_table* mt = get_main_table(test);
    print_main_table(mt);
    // block* b = get_block("tmp.txt");

    // char* a = calloc(9, sizeof(char));
    // a[0] = 't';
    // a[11] = 't';
    // printf("%p\n", a);
    // char* b = (char*) realloc(a, 12);
    // printf("%p\n", a);
    // printf("%p\n", b);


    // main_table mt = get_main_table(test);
    // int k = mt.blocks_num;
    // main_table* tmp = realloc(&mt, k + 1);
    return 0;
}