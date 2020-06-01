#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LETTERS  "abcdefghijklmnopqrstuvwxyz"
#define LETTERS_LEN  26
#define LINES_NUM 6
#define LINE_LEN 8
#define PRINTY_BOI "PRINTY BOI"

void concatenate(char** original, char* to_con) {
    char* tmp = calloc(strlen(*original) + strlen(to_con), sizeof(char));
    strcpy(tmp, *original);
    strcat(tmp, to_con);
    *original = tmp;
}

char* random_line() {
    char* line = calloc(LINE_LEN + 1, sizeof(char));
    for (int i = 0; i < LINE_LEN; i++) 
        line[i] = LETTERS[rand() % LETTERS_LEN];
    return line;
}

int main(int argc, char** argv) {
    int similarity_percent = atio(argv[1]);
    char* format = ".txt";
    for (int i = 0; i < LETTERS_LEN; i++) {
        char* folder_path = "./text_files/";
        char* file_path = calloc(strlen(folder_path) + 1, sizeof(char));
        strcpy(file_path, folder_path);
        char name[1];
        name[0] = LETTERS[i];
        concatenate(&file_path, name);
        concatenate(&file_path, format);


        if (i % 2 == 1) {
            FILE* f = fopen(file_path, "w");    
            char* prev_file = calloc(strlen(file_path) + 1, sizeof(char));
            strcpy(prev_file, file_path);
            prev_file[strlen(folder_path)] = LETTERS[i - 1];
            FILE* pf = fopen(prev_file, "w");
            
            for (int j = 0; j < LINES_NUM; j++) {
                char* line = random_line();
                fprintf(f, "%s\n", line);

                if (rand() % 100 > similarity_percent) {
                    fprintf(pf, "%s\n", line);
                } else {
                    char* pline = random_line();
                    fprintf(pf, "%s\n", pline);
                }
            }
            fclose(pf);
            fclose(f);
            
        }
    }
    
}