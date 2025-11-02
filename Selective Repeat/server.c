#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdbool.h>

#define PORT 8080
#define MAX_FRAMES 10
#define LOST_FRAME 3

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv = {0}, cli = {0};
    socklen_t len = sizeof(cli);
    bool recv_buf[MAX_FRAMES] = {false};
    bool lost = false;

    serv.sin_family = AF_INET;
    serv.sin_port = htons(PORT);
    serv.sin_addr.s_addr = INADDR_ANY;

    bind(sockfd, (struct sockaddr*)&serv, sizeof(serv));
    listen(sockfd, 1);
    int newsock = accept(sockfd, (struct sockaddr*)&cli, &len);
    printf("Server ready.\n");

    char buf[16];
    while (1) {
        memset(buf, 0, sizeof(buf));
        int n = recv(newsock, buf, sizeof(buf), 0);
        if (n <= 0) continue;
        if (!strncmp(buf, "END", 3)) break;

        int f = atoi(buf);
        if (!lost && f == LOST_FRAME) { lost = true; continue; }
        recv_buf[f] = true;

        char ack[8];
        sprintf(ack, "ACK%d", f);
        send(newsock, ack, strlen(ack) + 1, 0);
        printf("Got frame %d, sent %s\n", f, ack);
    }
    close(newsock);
    close(sockfd);
}
