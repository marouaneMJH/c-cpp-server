#include "../include/server/server.h"
#include "../include/http/route.h"
#include "../include/http/response.h"
#include <stdio.h>

ROUTE_GET("index", {
    printf("\nrun the handler\n");
    send_file("www/index.html", client_fd);
})

int main(void)
{

    // Initial the Route Table
    routeTable.get_routes.route_count = 0;
    routeTable.post_routes.route_count = 0;

    socket_tcp();

    return 0;
}