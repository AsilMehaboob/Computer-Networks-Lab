#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 1243
#define WINSIZE 3
#define LOCALHOST inet_addr("127.0.0.1")

typedef struct { int fnum; char data[30]; } Frame;
typedef struct { int anum; } Ack;

int main() {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in serv = {0};
    serv.sin_family = AF_INET;
    serv.sin_port = htons(PORT);
    serv.sin_addr.s_addr = LOCALHOST;

    struct timeval timeout = {3, 0};
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    Frame f; Ack a; socklen_t len = sizeof(serv);
    int base = 1, next = 1;

    while (base <= 6) {
        // Send all frames in window
        while (next < base + WINSIZE && next <= 6) {
            f.fnum = next;
            sprintf(f.data, "Frame %d", next);
            sendto(sock, &f, sizeof(f), 0, (struct sockaddr*)&serv, len);
            printf("[Client] Sent F%d\n", next++);
        }

        // Wait for ACK or timeout
        if (recvfrom(sock, &a, sizeof(a), 0, (struct sockaddr*)&serv, &len) > 0) {
            printf("[Client] ACK%d received\n", a.anum);
            base = a.anum + 1;
        } else {
            printf("[Client] Timeout → Resending from %d\n", base);
            next = base;
        }
    }
}
