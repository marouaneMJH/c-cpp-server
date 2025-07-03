#include "../include/server/server.h"
#include "../include/http/route.h"
#include "../include/http/response.h"

void hello_handler(int client_fd)
{
    send_file("www/index.html", client_fd);
}
int main(void)
{

    // Initial the Route Table
    routeTable.get_routes.route_count = 0;
    routeTable.post_routes.route_count = 0;

    app_get("/index", hello_handler);

    socket_tcp();

    return 0;
}