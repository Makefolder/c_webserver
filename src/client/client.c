#include "../file/files.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>

void handle_client(int sockfd)
{
    int bufsize = 512;
    char *buffer = (char *)malloc(sizeof(char)*bufsize);
    if (buffer == NULL)
    {
        perror("failed allocating memory");
        exit(EXIT_FAILURE);
    }

    int bytes_received = recv(sockfd, buffer, bufsize-1, 0);
    if (bytes_received < 0)
    {
        perror("failed receiving from clientfd\n");
        free(buffer);
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    buffer[bytes_received] = '\0';

    // routing
    printf("buffer: \n%s\n\n", buffer); // causing error
    if (strncmp(buffer, "GET /styles.css ", 16) == 0) {
        serve_file(sockfd, "./web/styles.css", "200 OK", "text/css\n"
            "Content-Length: 413");
    } else if (strncmp(buffer, "GET / ", 6) == 0) {
        serve_file(sockfd, "./web/index.html", "200 OK", "text/html\n"
            "Content-Length: 839");
    } else {
        // send_response(sockfd, not_found_response);
        serve_file(sockfd, "./web/not_found.html", "400 Not Found", "text/html\n"
            "Content-Length: 1101");
    }
    free(buffer);
    close(sockfd);
}

void send_response(int sockfd, char *response)
{
    int result_writing = write(sockfd, response, strlen(response));
    if (result_writing == 0)
        printf("nothing has been written to clientfd.\n");
    else if (result_writing == -1)
    {
        perror("failed writing to clientfd\n");
        exit(EXIT_FAILURE);
    }
    printf("\nresponse:\n%s\n", response);
}
