#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>

// lb - lower bound up - upper
void gen_matrix(char *f_path, int height, int width, int lb, int up) {
    FILE* f = fopen(f_path, "w");
    char* buffer = calloc(12, sizeof(char));  // 12 is enough for any int
    int written;
    written = sprintf(buffer, "%dx%d\n", height, width);
    fwrite(buffer, sizeof(char), written, f);

    srand(time(NULL));
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            int r = rand() % (up - lb + 1) + lb;
            written = sprintf(buffer, "%d\t", r);
            fwrite(buffer, sizeof(char), written, f);
        }
        fwrite("\n", sizeof(char), 1, f);
    }
}

int main() {
    int i = 2;
    char* tmp = "mt.txt";
    gen_matrix(tmp, 2, 3, 3, 10);
    return 0;
}