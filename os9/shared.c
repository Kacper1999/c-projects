#include <time.h>
#include <stdlib.h>
#include <stdbool.h>


int rand_int(int a, int b) {
    return rand() % (b - a) + a;
}