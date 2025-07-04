#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/types.h>

#include "../../include/server/server.h"
#include "../../include/server/config.h"
#include "../../include/http/parser.h"
#include "../../include/http/response.h"
#include "../../include/common/cli_style.h"
#include "../../include/http/route.h"

// Structure to pass data to the thread
typedef struct
{
    int server_fd;
    int client_fd;
    char *buffer;
} thread_args_t;

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
    if (listen(server_fd, 10000) < 0)
    {
        perror("listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf(FG_GREEN "[OK]" RESET " Server is listening on http://localhost:%d\n", PORT);

    while (1)
        manage_client_request_thread(server_fd);
    close(server_fd);
    return 0;
}

void manage_client_request(int client_fd)
{
    Request req = read_request(client_fd);

    // Serve the client
    serve_client(client_fd, req);

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

    printf(FG_CYAN "[+]" RESET " Client connected.\n");
    //    inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    return client_fd;
}
