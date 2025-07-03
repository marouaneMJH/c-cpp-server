#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

#include "parser.h"
#include "route.h"
/**
 * @brief Send a file as an HTTP response
 *
 * @param file_path Path to the file to send
 * @param client_fd Client socket file descriptor
 */
void send_file(char *file_path, int client_fd);

void send_404(int client_fd);

/**
 * @brief Serve a client with the default page
 *
 * @param client_fd Client socket file descriptor
 */
void serve_client(int client_fd, Request req);

#endif /* HTTP_RESPONSE_H */
