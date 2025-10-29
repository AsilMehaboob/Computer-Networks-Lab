#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080
#define LOCALHOST "127.0.0.1"
#define BUFFER_SIZE 256

struct sockaddr_in create_client_address(const char *server_ip, int port)
{
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(server_ip);

    return addr;
}

void error_check(int x, char success[])
{
    if (x < 0)
    {
        perror("something went wrong");
        exit(1);
    }
    else
    {
        printf("%s\n", success);
    }
}

int main()
{
    // 1. Create client socket
    int s = socket(AF_INET, SOCK_DGRAM, 0);
    error_check(s, "socket created");

    // 2. Build server address structure
    struct sockaddr_in server_address = create_client_address(LOCALHOST, PORT);
    socklen_t server_len = sizeof(server_address);

    // 3. Prepare buffers and request message
    char buffer[BUFFER_SIZE] = "";
    char request[] = "\n ... Hello, this is the Client ... \n";

    // 4. Send request to server
    int status = sendto(s, request, strlen(request), 0, (struct sockaddr *)&server_address, server_len);
    error_check(status, "message successfully sent to server");

    // 5. Receive reply from server
    status = recvfrom(s, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&server_address, &server_len);
    error_check(status, buffer);

    // 6. Close the socket and exit
    close(s);
    return 0;
}