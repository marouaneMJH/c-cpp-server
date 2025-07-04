#ifndef HANDLER
#define HANDLER

#include "./../http/parser.h"
#include "./../http/response.h"

void handle_index(int client_fd, Request *req);

void handle_404(int client_fd, Request *req);

void handle_about(int client_fd, Request *req);

void handle_project(int client_fd, Request *req);

#endif
