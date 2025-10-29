
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

struct sockaddr_in create_client_address()
{
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = inet_addr(LOCALHOST);
    
    return addr;
}

int main()
{
    // 1. Create client socket
    int s = socket(AF_INET, SOCK_STREAM, 0);
    error_check(s, "socket created");

    // 2. Build server address structure
    struct sockaddr_in server_address = create_client_address();

    // 3. Connect to the server
    int c = connect(s, (struct sockaddr *)&server_address, sizeof(server_address));
    error_check(c, "connecte to server");

    // 4. Prepare buffers and request message
    char buffer[BUFFER_SIZE] = "";
    char request[] = "\n ... Hello, this is the Client ... \n";

    // 5. Send request to server
    int status = send(s, request, strlen(request), 0);
    error_check(status, "message successfully sent to server");

    // 6. Receive reply from server
    status = recv(s, buffer, BUFFER_SIZE, 0);
    error_check(status, buffer);

    // 7. Close the socket and exit
    close(s);
    
    return 0;
}
