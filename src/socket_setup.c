// File: step2_accept_read.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/types.h>

#include "./../includes/socket_setup.h"
#include "./../includes/index.h"

#define PORT 8080
#define BUFFER_SIZE 1024

typedef enum
{
    POST,
    GET,
    PATCH,
    PUT,
    DELETE,

} RequestMethod;

typedef enum
{
    V1_1,
    V1_2,

} HTTPVersion;

typedef struct
{
    RequestMethod method;
    char *end_point;
    HTTPVersion version;
    char *user_agent;

} Request;

// Structure to pass data to the thread
typedef struct
{
    int server_fd;
    int client_fd;
    char *buffer;
} thread_args_t;

const char *response =
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/html\r\n"
    "Connection: close\r\n"
    "\r\n"
    "<html><body><h1>Hello, World!</h1></body></html>\r\n";

int serve_client(int client_fd)
{
    return write(client_fd, response, strlen(response));
}

Request read_request(int client_fd);

int accept_client_connection(int server_fd);

void manage_client_request(int client_fd);

// Thread function to handle client requests
void *client_handler(void *arg)
{
    thread_args_t *args = (thread_args_t *)arg;
    manage_client_request(args->client_fd);
    free(args->buffer); // Free the buffer allocated for this thread
    free(args);         // Free the arguments structure
    return NULL;
}

void manage_client_request_thread(int server_fd)
{
    int client_fd = accept_client_connection(server_fd);

    pthread_t client_thread_id;
    thread_args_t *args = malloc(sizeof(thread_args_t));
    if (args == NULL)
    {
        perror("Failed to allocate memory for thread args");
        return;
    }

    args->server_fd = server_fd;
    args->client_fd = client_fd;
    args->buffer = malloc(BUFFER_SIZE);

    if (args->buffer == NULL)
    {
        perror("Failed to allocate memory for buffer");
        free(args);
        return;
    }

    if (pthread_create(&client_thread_id, NULL, client_handler, args) != 0)
    {
        perror("Failed to create thread");
        free(args->buffer);
        free(args);
        return;
    }

    // Detach thread to automatically clean up when it terminates
    pthread_detach(client_thread_id);
}

Request extract_request(char *str_request)
{
    Request req;
    req.end_point = NULL;
    req.user_agent = NULL;

    // Parse the request line (first line)
    char *line = strtok(str_request, "\n");
    if (line)
    {
        // Parse method
        if (strncmp(line, "GET", 3) == 0)
            req.method = GET;
        else if (strncmp(line, "POST", 4) == 0)
            req.method = POST;
        else if (strncmp(line, "PUT", 3) == 0)
            req.method = PUT;
        else if (strncmp(line, "DELETE", 6) == 0)
            req.method = DELETE;
        else if (strncmp(line, "PATCH", 5) == 0)
            req.method = PATCH;

        // Parse endpoint
        char *endpoint_start = strchr(line, ' ');
        if (endpoint_start)
        {
            endpoint_start++; // Skip the space
            char *endpoint_end = strchr(endpoint_start, ' ');
            if (endpoint_end)
            {
                int length = endpoint_end - endpoint_start;
                req.end_point = malloc(length + 1);
                if (req.end_point)
                {
                    strncpy(req.end_point, endpoint_start, length);
                    req.end_point[length] = '\0';
                }

                // Parse HTTP version
                if (strstr(endpoint_end, "HTTP/1.1"))
                    req.version = V1_1;
                else if (strstr(endpoint_end, "HTTP/1.2"))
                    req.version = V1_2;
            }
        }
    }

    // Parse headers for User-Agent
    line = strtok(NULL, "\n");
    while (line)
    {
        if (strncmp(line, "User-Agent:", 11) == 0)
        {
            char *ua_start = line + 11;
            // Skip leading spaces
            while (*ua_start == ' ')
                ua_start++;

            int length = strlen(ua_start);
            req.user_agent = malloc(length + 1);
            if (req.user_agent)
            {
                strcpy(req.user_agent, ua_start);
                // Remove trailing CR if present
                if (length > 0 && req.user_agent[length - 1] == '\r')
                    req.user_agent[length - 1] = '\0';
            }
            break;
        }
        line = strtok(NULL, "\n");
    }

    return req;
}

int socket_tcp()
{
    int server_fd;
    struct sockaddr_in server_addr;

    // 1. Create TCP socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // 2. Prepare server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY; // Listen on all interfaces
    server_addr.sin_port = htons(PORT);

    // 3. Bind socket to address and port
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // 4. Listen for incoming connections
    if (listen(server_fd, 10) < 0)
    {
        perror("listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("[OK] Server is listening on http://localhost:%d\n", PORT);

    while (1)
        manage_client_request_thread(server_fd);
    close(server_fd);
    return 0;
}

void manage_client_request(int client_fd)
{
    Request req = read_request(client_fd);

    //
    if (req.end_point)
        printf("\nTTT%sTTT\n", req.end_point);

    // Serve the client
    serve_client(client_fd);

    // End connection with the client
    close(client_fd);
}

int accept_client_connection(int server_fd)
{
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
    if (client_fd < 0)
    {
        perror("accept failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("[+] Client connected: %s:%d\n",
           inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    return client_fd;
}

Request read_request(int client_fd)
{
    Request empty_request = {0}; // Initialize empty request for error cases

    // Allocate the memory for the user request
    char *req = (char *)malloc(BUFFER_SIZE * sizeof(char));

    // Check for the Allocation
    if (!req)
        return empty_request;

    // Read the Request
    ssize_t bytes_read = read(client_fd, req, BUFFER_SIZE - 1);

    if (bytes_read < 0)
    {
        perror("read failed");
        free(req);
        return empty_request;
    }

    // Null-terminate the string
    req[bytes_read] = '\0';

    // Extract the request
    Request result = extract_request(req);

    // Free the memory
    free(req);

    return result;
}