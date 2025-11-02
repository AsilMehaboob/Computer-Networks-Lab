#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdbool.h>

#define PORT 1243
#define LOCALHOST inet_addr("127.0.0.1")
#define WINSIZE 3
#define TOTAL_FRAMES 6

typedef struct { int fnum; char data[30]; } Frame;
typedef struct { int anum; } Ack;

int main() {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in serv = {0}, cli = {0};
    serv.sin_family = AF_INET;
    serv.sin_port = htons(PORT);
    serv.sin_addr.s_addr = LOCALHOST;
    bind(sock, (struct sockaddr*)&serv, sizeof(serv));

    Frame f; Ack a;
    socklen_t len = sizeof(cli);
    bool received[TOTAL_FRAMES + 1] = {false};
    int expected = 1;

    while (expected <= TOTAL_FRAMES) {
        if (recvfrom(sock, &f, sizeof(f), 0, (struct sockaddr*)&cli, &len) > 0) {
            printf("[Server] F%d received\n", f.fnum);

            // Simulate random frame loss (30% chance)
            if (rand() % 100 < 30) {
                printf("[Server] F%d dropped (simulated loss)\n", f.fnum);
                continue;
            }

            // Accept and buffer frame
            if (!received[f.fnum]) {
                received[f.fnum] = true;
                printf("[Server] F%d accepted\n", f.fnum);
            }

            // Send ACK for this frame (even if out of order)
            a.anum = f.fnum;
            sendto(sock, &a, sizeof(a), 0, (struct sockaddr*)&cli, len);
            printf("[Server] ACK%d sent\n", a.anum);

            // Slide window if next expected frames are ready
            while (received[expected] && expected <= TOTAL_FRAMES)
                expected++;
        }
    }

    printf("[Server] All frames received successfully.\n");
}
