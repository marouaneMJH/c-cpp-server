#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../../include/http/response.h"
#include "../../include/common/cli_style.h"

// private

// Find the  handler
void execute_handler(Request req, int client_fd)
{
    switch (req.method)
    {
    case GET:
        for (int i = 0; i < routeTable.get_routes.route_count; i++)
        {
            if (strcmp(routeTable.get_routes.routes[i].path, req.end_point) == 0)
            {
                routeTable.get_routes.routes[i].handler(client_fd, &req);
            }
        }
        break;
    case POST:
        for (int i = 0; i < routeTable.post_routes.route_count; i++)
        {
            if (strcmp(routeTable.post_routes.routes[i].path, req.end_point) == 0)
            {
                routeTable.post_routes.routes[i].handler(client_fd, &req);
            }
        }
        break;
    default:
        printf(FG_RED "handler not found for the %d\n" RESET, client_fd);
        break;
    }
}

// =======================================================
// =======================PUBLIC=======================
// =======================================================

void serve_client(int client_fd, Request req)
{
    execute_handler(req, client_fd);
    // If no route matched
    send_404(client_fd);
}

void send_404(int client_fd)
{
    send_file("www/404.html", client_fd);
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
