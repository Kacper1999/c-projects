#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


void t(const char str[256]) {
    printf("%s\n", str);
}


int main() {
    int msg_size = 256;
    char serv_msg[msg_size];
    printf("%d, %lu\n", AF_UNIX, sizeof(serv_msg));

    t(serv_msg);
    t("hi");
    char *tmp = malloc(3);
    tmp[0] = 'a';
    tmp[1] = 'b';
    tmp[2] = '\0';
    t(tmp);

    return 0;
}