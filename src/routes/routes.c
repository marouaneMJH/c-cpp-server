#include "./../../include/routes/routes.h"

void routes_connects()
{
    app_get("/home", handle_index);
    app_get("/", handle_index);
    app_get("/styles.css", handle_style);
    app_get("/about", handle_about);
    app_get("/project", handle_project);
    app_get("/favicon.ico", handle_favicon);
}