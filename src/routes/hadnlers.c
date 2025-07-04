#include "./../../include/routes/hadnlers.h"

void handle_index(int client_fd, Request *req)
{
    (void)req;
    const char *files[] = {"www/public/index.html", "www/public/style.css"};
    send_files(files, 2, client_fd);
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