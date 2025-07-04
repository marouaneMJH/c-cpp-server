#include "../include/server/server.h"
#include "../include/http/route.h"
#include "../include/http/response.h"
#include "../include/routes/routes.h"
#include <stdio.h>

int main(void)
{

    // // Initial the Route Table
    // routeTable.get_routes.route_count = 0;
    // routeTable.post_routes.route_count = 0;

    // routes_connects();
    // app routes

    socket_tcp();

    return 0;
}