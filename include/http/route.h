#ifndef ROUTE
#define ROUTE

#include "parser.h"

// MACROS
#define ROUTE_GET(path, body)                                               \
    static void __handler_##__LINE__(int client_fd, Request *req)           \
    {                                                                       \
        (void)req; /* prevents unused warning */                            \
        body                                                                \
    }                                                                       \
    static void __register_route_##__LINE__() __attribute__((constructor)); \
    static void __register_route_##__LINE__()                               \
    {                                                                       \
        app_get(path, __handler_##__LINE__);                                \
    }

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