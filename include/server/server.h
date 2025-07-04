#ifndef SERVER_H
#define SERVER_H

/**
 * @brief Initializes and runs a TCP socket server
 *
 * @return int Status code (0 for success, non-zero for failure)
 */
int socket_tcp(void);

/**
 * @brief Accepts a client connection
 *
 * @param server_fd Server socket file descriptor
 * @return int Client socket file descriptor
 */
int accept_client_connection(int server_fd);

/**
 * @brief Manages a client request
 *
 * @param client_fd Client socket file descriptor
 */
void manage_client_request(int client_fd);

/**
 * @brief Manages a client request in a separate thread
 *
 * @param server_fd Server socket file descriptor
 */
void manage_client_request_thread(int server_fd);

#endif /* SERVER_H */
