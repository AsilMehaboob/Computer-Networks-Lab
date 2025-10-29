
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>

#define PORT 8080
#define BUFFER_SIZE 256
#define LOCALHOST "127.0.0.1"

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

struct sockaddr_in create_socket_address()
{
    struct sockaddr_in socket_address;
    socket_address.sin_family = AF_INET;
    socket_address.sin_addr.s_addr = inet_addr(LOCALHOST);
    socket_address.sin_port = htons(PORT);

    return socket_address;
}

char *get_time()
{
    time_t t = time(NULL);
    return ctime(&t);
}

char* read_file()
{
    static char c[4096];

    FILE *f = fopen("sample.txt", "r");

    sprintf(c, "Filename: sample.txt\n\n");

    size_t bytes_read = fread(c + strlen(c), 1, 4095 - strlen(c), f);
    c[strlen(c) + bytes_read] = '\0';
    
    fclose(f);

    return c;
}

int main()
{
    // 1. Create server socket
    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    error_check(server_sock, "Server socket created successfully");

    // 2. Bind the socket to the server address
    struct sockaddr_in server_address = create_socket_address();
    socklen_t server_len = sizeof(server_address);
    int b = bind(server_sock, (struct sockaddr *)&server_address, server_len);
    error_check(b, "Binding successful");

    // 3. Listen for incoming connections
    int l = listen(server_sock, 5);
    error_check(l, "Listening...");

    // 4. Accept a client connection
    struct sockaddr_in client_address;
    socklen_t client_len = sizeof(client_address);
    int client_sock = accept(server_sock, (struct sockaddr *)&client_address, &client_len);
    error_check(client_sock, "Accepted connection from client");

    // 5. Communication with client
    char buffer[BUFFER_SIZE] = "";
    char reply[] = "\n ... Hello, this is the Server ...\n";
    // char *reply = get_time();
    // char *reply = read_file();

    int status = recv(client_sock, buffer, BUFFER_SIZE, 0);
    error_check(status, buffer);

    status = send(client_sock, reply, strlen(reply), 0);
    error_check(status, "Reply successfully sent to client");

    // 6. Close sockets
    close(client_sock);
    close(server_sock);

    return 0;
}
