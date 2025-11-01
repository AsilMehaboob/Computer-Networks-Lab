#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#define PORT 1243
#define LOCALHOST inet_addr("127.0.0.1")

typedef struct { int fnum; char data[30]; } Frame;
typedef struct { int anum; } Ack;

int main() {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in serv = {0}, cli = {0};
    serv.sin_family = AF_INET;
    serv.sin_port = htons(PORT);
    serv.sin_addr.s_addr = LOCALHOST;
    bind(sock, (struct sockaddr*)&serv, sizeof(serv));

    Frame f; Ack a; socklen_t len = sizeof(cli);
    int expected = 1;

    while (expected <= 6) {
        if (recvfrom(sock, &f, sizeof(f), 0, (struct sockaddr*)&cli, &len) > 0) {
            printf("[Server] F%d received\n", f.fnum);

            // Simulate frame loss
            if (rand() % 100 < 40) {
                printf("[Server] F%d dropped\n", f.fnum);
                continue;
            }

            if (f.fnum == expected) {
                a.anum = f.fnum;
                sendto(sock, &a, sizeof(a), 0, (struct sockaddr*)&cli, len);
                printf("[Server] ACK%d sent\n", a.anum);
                expected++;
            } else {
                a.anum = expected - 1;
                sendto(sock, &a, sizeof(a), 0, (struct sockaddr*)&cli, len);
                printf("[Server] ACK%d resent\n", a.anum);
            }
        }
    }
}
