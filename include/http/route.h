#ifndef ROUTE
#define ROUTE

#include "parser.h"
#include "macros.h"

#define MAX_ROUTES 128

typedef void (*RouteHandler)(int client_fd, Request *req);

typedef struct
{
    RequestMethod method; // GET, POST, etc
    char path[128];       // "/hello"
    RouteHandler handler;
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

void app_get(const char *path, RouteHandler handler);

void app_post(const char *path, RouteHandler handler);

#endif