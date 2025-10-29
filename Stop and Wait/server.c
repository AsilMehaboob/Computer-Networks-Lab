#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>

#define PORT 5000
#define MAXLINE 1024
#define TIMEOUT 5

int main() {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in servaddr = {0};
    socklen_t len = sizeof(servaddr);
    int frame = 0;
    char buffer[MAXLINE], ack[10];
    char* frames[] = {"Frame 1","Frame 2","Frame 3","Frame 4","Frame 5","Frame 6"};

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 

    struct timeval tv = {TIMEOUT, 0};
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    for (int i = 0; i < 6; ) {
        // send frame with colon separator (matches receiver’s sscanf)
        sprintf(buffer, "%d:%s", frame, frames[i]);
        sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr*)&servaddr, len);
        printf("\n[Sender] Sent: %s\n", buffer);

        int n = recvfrom(sockfd, buffer, MAXLINE, 0, (struct sockaddr*)&servaddr, &len);
        if (n < 0) {
            printf("[Sender] Timeout! Resending frame...\n");
            continue;
        }

        buffer[n] = '\0';
        sprintf(ack, "ACK%d", frame);

        if (!strcmp(buffer, ack)) {
            printf("[Sender] Received: %s\n", buffer);
            frame = 1 - frame;  // toggle frame
            i++;
        } else {
            printf("[Sender] Wrong ACK: %s, Resending...\n", buffer);
        }
    }

    close(sockfd);
    printf("\nAll Frames Done\n");
    return 0;
}
