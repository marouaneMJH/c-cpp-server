#ifndef HTTP_ROUTES_MACROS_G
#define HTTP_ROUTES_MACROS_G

// Helper macros for proper token concatenation
#define CONCAT_IMPL(a, b) a##b
#define CONCAT(a, b) CONCAT_IMPL(a, b)

// The key fix: Use the macro at GLOBAL scope, not inside functions!
#define ROUTE_GET(path, body)                                                    \
    static void CONCAT(handler_, __LINE__)(int client_fd, Request *req)          \
    {                                                                            \
        (void)req;                                                               \
        body                                                                     \
    }                                                                            \
    __attribute__((constructor)) static void CONCAT(register_route_, __LINE__)() \
    {                                                                            \
        app_get(path, CONCAT(handler_, __LINE__));                               \
    }

#define ROUTE_POST(path, body)                                                   \
    static void CONCAT(handler_, __LINE__)(int client_fd, Request *req)          \
    {                                                                            \
        (void)req;                                                               \
        body                                                                     \
    }                                                                            \
    __attribute__((constructor)) static void CONCAT(register_route_, __LINE__)() \
    {                                                                            \
        app_post(path, CONCAT(handler_, __LINE__));                              \
    }

#endif
