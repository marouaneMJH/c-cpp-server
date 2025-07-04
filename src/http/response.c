#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <limits.h>
#include <errno.h>

#include "../../include/http/response.h"
#include "../../include/common/cli_style.h"

#define BUFFER_SIZE 8192
#define MAX_HEADER_SIZE 1024
#define DEFAULT_FILE "www/index.html"
#define WEBROOT "www/"
#define MAX_FILE_SIZE (100 * 1024 * 1024) // 100MB limit
#define MAX_JSON_SIZE (10 * 1024 * 1024)  // 10MB limit

// Helper function to check if path is safe (within webroot)
static int is_safe_path(const char *file_path)
{
    if (!file_path)
        return 0;

    // Check for dangerous sequences
    if (strstr(file_path, "..") ||
        strstr(file_path, "//") ||
        file_path[0] == '/' ||
        strchr(file_path, '\0') != file_path + strlen(file_path))
    {
        return 0;
    }

    // Ensure path starts with webroot
    if (strncmp(file_path, WEBROOT, strlen(WEBROOT)) != 0)
    {
        return 0;
    }

    return 1;
}

// Helper function to get file size safely
static long get_file_size(FILE *fp)
{
    struct stat st;
    if (fstat(fileno(fp), &st) == 0)
    {
        return st.st_size;
    }
    return -1;
}

// Helper function to send error response
static void send_error_response(int client_fd, int status_code, const char *status_text)
{
    if (client_fd < 0)
        return;

    char error_response[512];

    int len = snprintf(error_response, sizeof(error_response),
                       "HTTP/1.1 %d %s\r\n"
                       "Content-Type: text/html\r\n"
                       "Content-Length: 0\r\n"
                       "Connection: close\r\n\r\n",
                       status_code, status_text);

    if (len > 0 && len < (int)sizeof(error_response))
        write(client_fd, error_response, len);
}

// Find the handler - FIXED: Added bounds checking and proper error handling
static void execute_handler(Request req, int client_fd)
{
    if (client_fd < 0)
        return;

    int handler_found = 0;

    switch (req.method)
    {
    case GET:
        if (routeTable.get_routes.route_count > 0)
        {
            for (int i = 0; i < routeTable.get_routes.route_count; i++)
            {
                if (req.end_point &&
                    strcmp(routeTable.get_routes.routes[i].path, req.end_point) == 0)
                {
                    if (routeTable.get_routes.routes[i].handler)
                    {
                        routeTable.get_routes.routes[i].handler(client_fd, &req);
                        handler_found = 1;
                    }
                    break;
                }
            }
        }
        break;
    case POST:
        if (routeTable.post_routes.route_count > 0)
        {
            for (int i = 0; i < routeTable.post_routes.route_count; i++)
            {
                if (req.end_point &&
                    strcmp(routeTable.post_routes.routes[i].path, req.end_point) == 0)
                {
                    if (routeTable.post_routes.routes[i].handler)
                    {
                        routeTable.post_routes.routes[i].handler(client_fd, &req);
                        handler_found = 1;
                    }
                    break;
                }
            }
        }
        break;
    default:
        printf(FG_RED "Unsupported method for client %d\n" RESET, client_fd);
        send_error_response(client_fd, 405, "Method Not Allowed");
        return;
    }

    if (!handler_found)
    {
        send_404(client_fd);
    }
}

// FIXED: Added input validation and safer string handling
static const char *get_mime_type(const char *filename)
{
    if (!filename)
        return "application/octet-stream";

    const char *ext = strrchr(filename, '.');
    if (!ext)
        return "application/octet-stream";

    // Use a lookup table for better performance and safety
    static const struct
    {
        const char *ext;
        const char *mime_type;
    } mime_types[] = {
        {".html", "text/html"},
        {".css", "text/css"},
        {".js", "application/javascript"},
        {".png", "image/png"},
        {".jpg", "image/jpeg"},
        {".jpeg", "image/jpeg"},
        {".svg", "image/svg+xml"},
        {".gif", "image/gif"},
        {".ico", "image/x-icon"},
        {".txt", "text/plain"},
        {".json", "application/json"},
        {".pdf", "application/pdf"},
        {NULL, NULL}};

    for (int i = 0; mime_types[i].ext; i++)
    {
        if (strcmp(ext, mime_types[i].ext) == 0)
        {
            return mime_types[i].mime_type;
        }
    }

    return "application/octet-stream";
}

// =======================================================
// =======================PUBLIC=======================
// =======================================================

// FIXED: Removed the double 404 call issue
void serve_client(int client_fd, Request req)
{
    if (client_fd < 0)
        return;
    execute_handler(req, client_fd);
    // Note: execute_handler now handles 404 internally if no route matches
}

void send_404(int client_fd)
{
    send_file("www/404.html", client_fd);
}

void send_500(int client_fd)
{
    send_file("www/500.html", client_fd);
}

// COMPLETELY REWRITTEN: Fixed all security vulnerabilities
void send_file(char *file_path, int client_fd)
{
    FILE *fp = NULL;
    char *buffer = NULL;
    char *safe_path = NULL;

    // Validate client_fd
    if (client_fd < 0)
    {
        return;
    }

    // Determine safe file path
    if (file_path && is_safe_path(file_path))
    {
        safe_path = file_path;
    }
    else if (file_path)
    {
        // Path provided but unsafe, send 404
        send_error_response(client_fd, 404, "Not Found");
        return;
    }
    else
    {
        safe_path = DEFAULT_FILE;
    }

    // Open file
    fp = fopen(safe_path, "rb");
    if (!fp)
    {
        send_error_response(client_fd, 404, "Not Found");
        return;
    }

    // Get file size safely
    long file_size = get_file_size(fp);
    if (file_size < 0)
    {
        fclose(fp);
        send_error_response(client_fd, 500, "Internal Server Error");
        return;
    }

    // Check for unreasonably large files
    if (file_size > MAX_FILE_SIZE)
    {
        fclose(fp);
        send_error_response(client_fd, 413, "Payload Too Large");
        return;
    }

    // Allocate buffer for streaming
    buffer = malloc(BUFFER_SIZE);
    if (!buffer)
    {
        fclose(fp);
        send_error_response(client_fd, 500, "Internal Server Error");
        return;
    }

    // Prepare header with bounds checking
    char header[MAX_HEADER_SIZE];
    const char *mime_type = get_mime_type(safe_path);

    int header_len = snprintf(header, sizeof(header),
                              "HTTP/1.1 200 OK\r\n"
                              "Content-Type: %s\r\n"
                              "Content-Length: %ld\r\n"
                              "Connection: close\r\n\r\n",
                              mime_type, file_size);

    if ((size_t)header_len >= sizeof(header))
    {
        fclose(fp);
        free(buffer);
        send_error_response(client_fd, 500, "Internal Server Error");
        return;
    }

    // Send header
    if (write(client_fd, header, header_len) != header_len)
    {
        fclose(fp);
        free(buffer);
        return;
    }

    // Send file content in chunks
    size_t bytes_read;
    long total_sent = 0;

    while (total_sent < file_size && (bytes_read = fread(buffer, 1, BUFFER_SIZE, fp)) > 0)
    {
        ssize_t bytes_written = write(client_fd, buffer, bytes_read);
        if (bytes_written != (ssize_t)bytes_read)
        {
            break; // Connection error
        }
        total_sent += bytes_written;
    }

    // Cleanup
    fclose(fp);
    free(buffer);
}

// FIXED: Added input validation and bounds checking
void send_json(const char *json, int client_fd)
{
    if (!json || client_fd < 0)
    {
        send_500(client_fd);
        return;
    }

    size_t json_len = strlen(json);

    // Check for reasonable JSON size
    if (json_len > MAX_JSON_SIZE)
    {
        send_error_response(client_fd, 413, "Payload Too Large");
        return;
    }

    char header[MAX_HEADER_SIZE];
    int header_len = snprintf(header, sizeof(header),
                              "HTTP/1.1 200 OK\r\n"
                              "Content-Type: application/json\r\n"
                              "Content-Length: %zu\r\n"
                              "Connection: close\r\n\r\n",
                              json_len);

    if ((size_t)header_len >= sizeof(header))
    {
        send_500(client_fd);
        return;
    }

    // Send header
    if (write(client_fd, header, header_len) != header_len)
    {
        return;
    }

    // Send JSON content
    write(client_fd, json, json_len);
}
