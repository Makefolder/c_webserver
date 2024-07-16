#include "./client/client.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>

int PORT;

int main(int argc, char *argv[])
{
    if (argc > 1)
        PORT = atoi(argv[1]);
    else
        PORT = 8080;

    // basic things
    int sockfd = create_socket(0);
    struct sockaddr_in server_addr = setup_local_addr(PORT);
    socklen_t addr_len = sizeof(server_addr);
    if (bind(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
    {
        perror("failed binding address to socket");
        exit(EXIT_FAILURE);
    }
    if (listen(sockfd, 10) < 0)
    {
        perror("failed listening to socket");
        exit(EXIT_FAILURE);
    }
    printf("server is up and running on port %d.\n", PORT);

    while (1)
    {
        int clientfd;
        if ((clientfd = accept(sockfd, (struct sockaddr *) &server_addr, &addr_len)) < 0)
        {
            perror("failed accepting clientfd");
            exit(EXIT_FAILURE);
        }
        handle_client(clientfd);
    }
    return EXIT_SUCCESS;
}
