#include "./../../include/routes/routes.h"

void routes_connects()
{
    app_get("/home", handle_index);
    app_get("/about", handle_about);
    app_get("/project", handle_project);
}