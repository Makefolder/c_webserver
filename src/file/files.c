#include "../client/client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
