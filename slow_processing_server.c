#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

// maximum application buffer
#define APP_MAX_BUFFER 1024
#define PORT 8080

int main()
{

    // define the server and client file descriptor
    int server_fd, client_fd;

    // socket address
    struct sockaddr_in address;
    int address_len = sizeof(address);

    // buffer initialize with 0
    char buffer[APP_MAX_BUFFER] = {0};

    // create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // binding this socket
    address.sin_family = AF_INET;         // ipv4
    address.sin_addr.s_addr = INADDR_ANY; // listening on 0.0.0.0
    address.sin_port = PORT;

    // SO_REUSEPORT can be use here
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // creating the queues : ACK, ACCCEPT, RECEIVE, SEND
    //  here 10 is the backlog or the size of queue it means it can hold only 10 connections
    if (listen(server_fd, 10) < 0)
    {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    // always run this loop
    while (1)
    {
        printf("\nWaiting for a connection...\n");

        // calling the accept function
        if ((client_fd = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&address_len)) < 0)
        {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }

        read(client_fd, buffer, APP_MAX_BUFFER);
        printf("%s\n", buffer);


        //simulate slow processing
        sleep(6); // sleep for 6 seconds and then send the request

        
        char const *http_response = "HTTP/1.1 200 OK\nContent-Type:text/plain\nContent-Length:13\n\nHello world!\n";

        // writing to the socket with response
        // send queue os
        write(client_fd, http_response, strlen(http_response));
        close(client_fd);
    }

    return 0;
}