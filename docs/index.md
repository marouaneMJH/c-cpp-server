# C HTTP Server Documentation

## Overview

This is a lightweight HTTP server implemented in C. It provides basic HTTP request handling, multithreading for concurrent connections, and file serving capabilities.

## Server Architecture

The server follows a modular architecture:

1. **Main Module**: Entry point that initializes the server
2. **Server Module**: Handles TCP socket connections and client request delegation
3. **HTTP Parser Module**: Parses incoming HTTP requests
4. **HTTP Response Module**: Generates and sends HTTP responses

## API Reference

### Server Module

-   `socket_tcp()`: Initializes the TCP server socket and starts the main event loop
-   `accept_client_connection(int server_fd)`: Accepts a new client connection
-   `manage_client_request(int client_fd)`: Processes a client request
-   `manage_client_request_thread(int server_fd)`: Creates a new thread to handle a client request

### HTTP Parser Module

-   `read_request(int client_fd)`: Reads and parses an HTTP request from a client socket
-   `extract_request(char *str_request)`: Extracts HTTP request data from a raw string

### HTTP Response Module

-   `serve_client(int client_fd)`: Serves the default content to a client
-   `send_file(char *file_path, int client_fd)`: Sends a file as an HTTP response

## Configuration

Server configuration parameters can be found in `include/server/config.h`:

-   `PORT`: The port number the server listens on (default: 8080)
-   `BUFFER_SIZE`: The buffer size for reading client requests (default: 1024)

## Extending the Server

To add new HTTP endpoints, modify the `serve_client` function in `src/http/response.c` to handle different request paths.
