#include "./../../include/routes/hadnlers.h"

void handle_index(int client_fd, Request *req)
{
    (void)req;
    send_file("www/public/index.html", client_fd);
}

void handle_style(int client_fd, Request *req)
{
    (void)req;

    send_file("www/public/styles.css", client_fd);
}

void handle_404(int client_fd, Request *req)
{
    (void)req;

    send_file("www/public/404.html", client_fd);
}

void handle_about(int client_fd, Request *req)
{
    (void)req;
    send_file("www/public/about.html", client_fd);
}

void handle_project(int client_fd, Request *req)
{
    (void)req;
    send_file("www/public/project.html", client_fd);
}

void handle_favicon(int client_fd, Request *req)
{
    (void)req;
    send_file("www/public/favicon.ico", client_fd);
}
