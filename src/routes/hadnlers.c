#include "./../../include/routes/hadnlers.h"

void handle_index(int client_fd, Request *req)
{
    (void)req;
    send_file("www/index.html", client_fd);
}

void handle_style(int client_fd, Request *req)
{
    (void)req;

    send_file("www/styles.css", client_fd);
}

void handle_404(int client_fd, Request *req)
{
    (void)req;

    send_file("www/404.html", client_fd);
}

void handle_about(int client_fd, Request *req)
{
    (void)req;
    send_file("www/about.html", client_fd);
}

void handle_project(int client_fd, Request *req)
{
    (void)req;
    send_file("www/project.html", client_fd);
}

void handle_favicon(int client_fd, Request *req)
{
    (void)req;
    send_file("www/favicon.ico", client_fd);
}

void handle_image(int client_fd, Request *req)
{
    (void)req;
    send_file("www/images/image.png", client_fd);
}
