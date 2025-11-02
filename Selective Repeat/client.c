#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdbool.h>

#define PORT 1243
#define WINSIZE 3
#define LOCALHOST inet_addr("127.0.0.1")
#define TOTAL_FRAMES 6

typedef struct { int fnum; char data[30]; } Frame;
typedef struct { int anum; } Ack;

int main() {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in serv = {0};
    serv.sin_family = AF_INET;
    serv.sin_port = htons(PORT);
    serv.sin_addr.s_addr = LOCALHOST;

    Frame f; Ack a;
    socklen_t len = sizeof(serv);
    bool acked[TOTAL_FRAMES + 1] = {false};
    int base = 1, next = 1;

    struct timeval timeout = {2, 0};
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    while (base <= TOTAL_FRAMES) {
        // Send all frames within window
        while (next < base + WINSIZE && next <= TOTAL_FRAMES) {
            if (!acked[next]) {
                f.fnum = next;
                sprintf(f.data, "Frame %d", next);
                sendto(sock, &f, sizeof(f), 0, (struct sockaddr*)&serv, len);
                printf("[Client] Sent F%d\n", next);
            }
            next++;
        }

        // Wait for ACKs
        if (recvfrom(sock, &a, sizeof(a), 0, (struct sockaddr*)&serv, &len) > 0) {
            printf("[Client] ACK%d received\n", a.anum);
            acked[a.anum] = true;

            // Move base to next unacked frame
            while (acked[base] && base <= TOTAL_FRAMES)
                base++;
        } else {
            // Timeout → resend only unacked frames in window
            printf("[Client] Timeout → Resending unacked frames from %d to %d\n", base, next - 1);
            for (int i = base; i < next; i++) {
                if (!acked[i]) {
                    f.fnum = i;
                    sprintf(f.data, "Frame %d", i);
                    sendto(sock, &f, sizeof(f), 0, (struct sockaddr*)&serv, len);
                    printf("[Client] Resent F%d\n", i);
                }
            }
        }
    }

    printf("[Client] All frames sent and acknowledged.\n");
}
