#include <stdio.h>

void serve_file(int sockfd, char *path, char *status, char *content_type);
void parse_file(char **response, int *len, char *headers, int header_size, FILE *file);
