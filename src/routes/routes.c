#include "./../../include/routes/routes.h"
#include <stdio.h>

ROUTE_GET("/image-2", {
    send_file("www/images/image.png", client_fd);
};)

ROUTE_GET("/project", {
    send_file("www/project.html", client_fd);
})

ROUTE_GET("/about", {
    // send_file("www/images/image.png", client_fd);
    handle_about(client_fd, req);
})

ROUTE_GET("/home", {
    handle_index(client_fd, req);
})

ROUTE_GET("/", {
    handle_index(client_fd, req);
})

ROUTE_GET("/styles.css", {
    handle_style(client_fd, req);
})

ROUTE_GET("/image", {
    handle_image(client_fd, req);
})
