#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int create_socket(int flag)
{
    int sockfd = socket(AF_INET, SOCK_STREAM, flag);
    return sockfd;
}

struct sockaddr_in setup_serv_addr(int port, char* address)
{
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (inet_aton(address, (struct in_addr *) &addr) == 0)
    {
        perror("invalid address.");
        exit(EXIT_FAILURE);
    }
    return addr;
}

struct sockaddr_in setup_local_addr(int port)
{
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    return addr;
}
