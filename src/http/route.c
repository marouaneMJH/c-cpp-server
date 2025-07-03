#include "./../../include/http/route.h"
#include <string.h>
#include <stdio.h>
// Define the global routeTable
RouteTable routeTable;

void app_get(const char *path, RouteHandler handler)
{

    if (routeTable.get_routes.route_count < MAX_ROUTES)
    {
        routeTable.get_routes.routes[routeTable.get_routes.route_count].method = GET;
        strcpy(routeTable.get_routes.routes[routeTable.get_routes.route_count].path, path);
        routeTable.get_routes.routes[routeTable.get_routes.route_count].handler = handler;
        routeTable.get_routes.route_count++;

        printf("\n%s added to the path.\n", path);
    }
}

void app_post(const char *path, RouteHandler handler)
{

    if (routeTable.post_routes.route_count < MAX_ROUTES)
    {
        routeTable.post_routes.routes[routeTable.post_routes.route_count].method = GET;
        strcpy(routeTable.post_routes.routes[routeTable.post_routes.route_count].path, path);
        routeTable.post_routes.routes[routeTable.post_routes.route_count].handler = handler;
        routeTable.post_routes.route_count++;
    }
}