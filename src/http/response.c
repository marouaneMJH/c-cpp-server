#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../../include/http/response.h"

void serve_client(int client_fd)
{
    send_file("www/index.html", client_fd);
}

void send_file(char *file_path, int client_fd)
{
    FILE *fp = fopen(file_path ? file_path : "www/index.html", "rb");
    if (!fp)
    {
        send_file("www/404.html", client_fd);
    }

    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    rewind(fp);

    char *file_content = malloc(file_size);
    fread(file_content, 1, file_size, fp);
    fclose(fp);

    char header[512];

    sprintf(header,
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: %ld\r\n"
            "Connection: close\r\n\r\n",
            file_size);

    write(client_fd, header, strlen(header));
    write(client_fd, file_content, file_size);

    free(file_content);
}
