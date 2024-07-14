#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define PORT 8080

int main(void)
{
    // basic things
    FILE * index, *styles;
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

    // receive/write to client's sockfd
    int bufsize = 1024;
    char* buffer = malloc(sizeof(char)*bufsize);
    while (1)
    {
        int clientfd;
        if ((clientfd = accept(sockfd, (struct sockaddr *) &server_addr, &addr_len)) < 0)
        {
            perror("failed accepting clientfd");
            exit(EXIT_FAILURE);
        }

        if (clientfd > 0)
            printf("new client has been connected.\n");
        if (recv(clientfd, buffer, bufsize, 0) < 0)
        {
            perror("failed receiving from clientfd\n");
            exit(EXIT_FAILURE);
        }
        printf("buffer: \n%s\n\n", buffer);

        // index.html file
        int len = 256;
        char *response = (char *)malloc(sizeof(char)*len);
        index = fopen("./web/index.html", "r");
        if (index == NULL)
        {
            perror("failed opening index.html file");
            response =
                "HTTP/1.1 200 OK\n"
                "Content-Type: text/html\n\n"
                "<html><body><h1>Internal error</h1></body></html>";
        } else
        {
            const char* headers =
                "HTTP/1.1 200 OK\n"
                "Content-Type: text/html\r\n\r\n";

            // append headers
            strcat(response, headers);

            char ch;
            int count_c = 0 + strlen(headers);
            // parse index.html
            while ((ch = fgetc(index)) != EOF)
            {
                response[count_c] = ch;
                if (count_c == len-1)
                {
                    len *= 2;
                    char *res = (char *)realloc(response, sizeof(char)*len);
                    if (res == NULL)
                    {
                        perror("failed reallocate memory");
                        close(sockfd);
                        fclose(index);
                        free(response);
                        free(buffer);
                        exit(EXIT_FAILURE);
                    }
                    response = res;
                }
                count_c++;
            }
            fclose(index);
        }
        // send response to client
        int result_writing = write(clientfd, response, strlen(response));
        if (result_writing == 0)
            printf("nothing has been written to clientfd.\n");
        else if (result_writing == -1)
        {
            perror("failed writing to clientfd\n");
            exit(EXIT_FAILURE);
        }
        free(response);
    }
    free(buffer);
    close(sockfd);
    return EXIT_SUCCESS;
}
