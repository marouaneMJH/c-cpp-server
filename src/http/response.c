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
            "Content-Type: %s\r\n"
            "Content-Length: %ld\r\n"
            "Connection: close\r\n\r\n",
            get_mime_type(file_path),
            file_size);

    write(client_fd, header, strlen(header));
    write(client_fd, file_content, file_size);

    free(file_content);
}

const char *get_mime_type(const char *filename)
{
    const char *ext = strrchr(filename, '.');
    if (!ext)
        return "application/octet-stream";

    if (strcmp(ext, ".html") == 0)
        return "text/html";
    if (strcmp(ext, ".css") == 0)
        return "text/css";
    if (strcmp(ext, ".js") == 0)
        return "application/javascript";
    if (strcmp(ext, ".png") == 0)
        return "image/png";
    if (strcmp(ext, ".jpg") == 0 || strcmp(ext, ".jpeg") == 0)
        return "image/jpeg";
    if (strcmp(ext, ".svg") == 0)
        return "image/svg+xml";

    return "application/octet-stream";
}

/*BUG not working cuz the http not support multi files*/
void send_files(const char **file_paths, int path_count, int client_fd)
{
    long total_size = 0;
    FILE **file_handles = malloc(path_count * sizeof(FILE *));
    long *file_sizes = malloc(path_count * sizeof(long));
    char **file_contents = malloc(path_count * sizeof(char *));
    int valid_files = 0;

    // Open each file and calculate total size
    for (int i = 0; i < path_count; i++)
    {
        if (file_paths[i])
        {
            file_handles[valid_files] = fopen(file_paths[i], "rb");
            if (file_handles[valid_files])
            {
                fseek(file_handles[valid_files], 0, SEEK_END);
                file_sizes[valid_files] = ftell(file_handles[valid_files]);
                rewind(file_handles[valid_files]);

                total_size += file_sizes[valid_files];
                valid_files++;
            }
        }
    }

    // If no files were found, serve 404
    if (valid_files == 0)
    {
        free(file_handles);
        free(file_sizes);
        free(file_contents);
        send_404(client_fd);
        return;
    }

    // Read all file contents
    for (int i = 0; i < valid_files; i++)
    {
        file_contents[i] = malloc(file_sizes[i]);
        fread(file_contents[i], 1, file_sizes[i], file_handles[i]);
        fclose(file_handles[i]);
    }

    // Send HTTP header
    char header[512];
    sprintf(header,
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: %ld\r\n"
            "Connection: close\r\n\r\n",
            total_size);
    write(client_fd, header, strlen(header));

    // Send all file contents
    for (int i = 0; i < valid_files; i++)
    {
        write(client_fd, file_contents[i], file_sizes[i]);
        free(file_contents[i]);
    }

    // Free allocated memory
    free(file_handles);
    free(file_sizes);
    free(file_contents);
}
