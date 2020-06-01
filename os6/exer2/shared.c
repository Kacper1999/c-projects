#include "shared.h"

int str_equal(char* str1, char* str2) { return strcmp(str1, str2) == 0; }

char* r_str(int length) {
    char* str = calloc(length + 1, sizeof(char));
    for (int i = 0; i < length; i++) {
        char r_char = 'a' + (rand() % 26);
        str[i] = r_char;
    }
    str[length] = '\0';
    return str;
}

char* concat(const char* s1, const char* s2) {
    int   len = strlen(s1) + strlen(s2) + 1;
    char* result = (char*)malloc(len);
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

char* get_cwd() { return getcwd(NULL, 0); }

void error() {
    if (errno != 0) {
        fprintf(stderr, "Value of errno: %d\n", errno);
        perror("Error printed by perror");
    }
}

int get_q(char* name) {
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_MSG_LENGTH - 1;
    attr.mq_curmsgs = 0;
    return mq_open(name, O_RDONLY | O_CREAT | O_EXCL, 0666, &attr);
}