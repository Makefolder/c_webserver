#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>

int PORT;

void send_response(int clientfd, char *response);
void serve_file(int sockfd, char *path, char *status, char *content_type);
void handle_client(int sockfd);
void parse_file(char **response, int *len, char *headers, int header_size, FILE *file);

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

void serve_file(int sockfd, char *path, char *status, char *content_type)
{
    int len = 256;
    char *response = (char *)malloc(sizeof(char)*len);
    if (response == NULL)
    {
        perror("failed allocating memory");
        exit(EXIT_FAILURE);
    }

    FILE *file = fopen(path, "r");
    char *headers = (char *)malloc(sizeof(char)*256);
    if (headers == NULL)
    {
        perror("failed allocating memory");
        exit(EXIT_FAILURE);
    }

    if (file == NULL)
    {
        perror("failed opening index.html file");
        // send_response(sockfd, not_found_response);
        snprintf(headers, 256,
            "HTTP/1.1 %s\nContent-Type: %s\nConnection: Closed\r\n\r\n",
            status, content_type);

        parse_file(&response, &len, headers, strlen(headers), file);
        fclose(file);
        free(headers);
    } else {
        snprintf(headers, 256,
            "HTTP/1.1 %s\nContent-Type: %s\nConnection: Closed\r\n\r\n",
            status, content_type);

        // append headers
        parse_file(&response, &len, headers, strlen(headers), file);
        fclose(file);
    }

    send_response(sockfd, response);
    free(headers);
    free(response);
}

void parse_file(char **response, int *len, char *headers, int header_size, FILE *file)
{
    int headers_len = strlen(headers);
    if (((*len)-1) <= headers_len)
    {
        *len *= 2;
        char *result = (char *)realloc(*response, *len);
        if (result == NULL)
        {
            perror("error reallocating memory");
            exit(EXIT_FAILURE);
        }
        *response = result;
    }
    // strcat(*response, headers); // error here
    strcpy(*response, headers);
    char ch;
    int count_c = 0 + header_size;
    if (file != NULL)
    {
        while ((ch = fgetc(file)) != EOF)
        {
            (*response)[count_c] = ch;
            if (count_c == *len-1)
            {
                *len *= 2;
                char *res = (char *)realloc(*response, sizeof(char)*(*len));
                if (res == NULL)
                {
                    perror("failed reallocating memory");
                    fclose(file);
                    free(*response);
                    exit(EXIT_FAILURE);
                }
                *response = res;
            }
            count_c++;
        }
        (*response)[count_c] = '\0';
    } else {
        *response = "HTTP/1.1 500 Internal Server Error"
            "Content-Type: text/html"
            "Content-Length: 79"
            "Connection: Close\r\n\r\n"
            "<html><body><h1>Internal Error</h1><a href=\"/\">take me back</a></body></html>";
    }
}
