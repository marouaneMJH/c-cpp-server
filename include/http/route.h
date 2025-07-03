#ifndef ROUTE
#define ROUTE

#include "parser.h"

#define MAX_ROUTES 128

typedef struct
{
    RequestMethod method;           // GET, POST, etc
    char path[128];                 // "/hello"
    void (*handler)(int client_fd); // callback to handle the request
} Route;

typedef struct
{
    Route routes[MAX_ROUTES];
    int route_count;
} RouteMethodTable;

typedef struct
{
    RouteMethodTable post_routes;
    RouteMethodTable get_routes;
} RouteTable;

// Global route table
extern RouteTable routeTable;

void app_get(const char *path, void (*handler)(int));

void app_post(const char *path, void (*handler)(int));

#endif