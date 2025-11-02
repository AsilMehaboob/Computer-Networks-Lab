#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdbool.h>

#define PORT 8080
#define MAX_FRAMES 10
#define WIN 4
#define TIMEOUT 3

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv = {0};
    serv.sin_family = AF_INET;
    serv.sin_port = htons(PORT);
    serv.sin_addr.s_addr = inet_addr("127.0.0.1");
    connect(sock, (struct sockaddr*)&serv, sizeof(serv));
    printf("Client connected.\n");

    bool acked[MAX_FRAMES] = {false};
    int base = 0, next = 0, timers[MAX_FRAMES] = {0};
    char msg[16], buf[16];

    while (base < MAX_FRAMES) {
        while (next < base + WIN && next < MAX_FRAMES) {
            sprintf(msg, "%d", next);
            send(sock, msg, sizeof(msg), 0);
            printf("Sent frame %d\n", next);
            timers[next] = 0;
            next++;
        }

        sleep(1);
        memset(buf, 0, sizeof(buf));
        if (recv(sock, buf, sizeof(buf), MSG_DONTWAIT) > 0) {
            int a = atoi(buf + 3);
            acked[a] = true;
            printf("Got ACK%d\n", a);
        }

        for (int i = base; i < next; i++)
            if (!acked[i] && ++timers[i] >= TIMEOUT) {
                sprintf(msg, "%d", i);
                send(sock, msg, sizeof(msg), 0);
                printf("Timeout -> Resent frame %d\n", i);
                timers[i] = 0;
            }

        while (base < MAX_FRAMES && acked[base]) base++;
    }

    send(sock, "END", 4, 0);
    printf("All frames sent.\n");
    close(sock);
}
