#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 5000
#define MAXLINE 1024

int main() {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in servaddr = {0}, cliaddr = {0};
    socklen_t len = sizeof(cliaddr);
    char buffer[MAXLINE], last_ack[10] = "";
    int expected = 0, frame_count = 0;

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    bind(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));

    printf("[Receiver] Waiting for frames...\n");



    while (1) {
        int n = recvfrom(sockfd, buffer, MAXLINE, 0, (struct sockaddr*)&cliaddr, &len);
        buffer[n] = '\0';
        
        int frame; char data[100];
        if (sscanf(buffer, "%d:%99[^\n]", &frame, data) != 2) continue;

        frame_count++;
        printf("\n[Receiver] Frame %d received: %s\n", frame, data);

        // Simulate packet loss on frame 3 and 6
        if (frame_count == 3 || frame_count == 6) {
            printf("[Receiver] Simulating packet loss for Frame %d\n", frame);
            continue;
        }

        if (frame == expected) {
            printf("[Receiver] Frame accepted\n");
            sprintf(buffer, "ACK%d", frame);
            sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr*)&cliaddr, len);
            printf("[Receiver] Sent: %s\n", buffer);
            strcpy(last_ack, buffer);
            expected = 1 - expected;
        } else {
            printf("[Receiver] Duplicate frame. Resending last ACK: %s\n", last_ack);
            if (*last_ack)
                sendto(sockfd, last_ack, strlen(last_ack), 0, (struct sockaddr*)&cliaddr, len);
        }
    }

    close(sockfd);
    return 0;
}
