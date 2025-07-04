#include "./../../include/routes/routes.h"
#include <stdio.h>
#include <math.h>
#include <time.h>

// Define a heavy function (like a probability density)
static double f(double x)
{
    return exp(-x * x) * sin(x * x) * sqrt(x + 1);
}

// Simpson's Rule for numerical integration
static double simpson_integration(double a, double b, long int n)
{
    if (n % 2 != 0)
        n++; // n must be even

    double h = (b - a) / n;
    double result = f(a) + f(b);

    for (int i = 1; i < n; i++)
    {
        double x = a + i * h;
        result += (i % 2 == 0) ? 2 * f(x) : 4 * f(x);
    }

    return result * h / 3.0;
}

ROUTE_GET("/image-2", {
    send_file("www/images/image.png", client_fd);
};)

ROUTE_GET("/project", {
    send_file("www/project.html", client_fd);
})

ROUTE_GET("/about", {
    // send_file("www/images/image.png", client_fd);
    handle_about(client_fd, req);
})

ROUTE_GET("/home", {
    handle_index(client_fd, req);
})

ROUTE_GET("/", {
    handle_index(client_fd, req);
})

ROUTE_GET("/styles.css", {
    handle_style(client_fd, req);
})

ROUTE_GET("/image", {
    handle_image(client_fd, req);
})

ROUTE_GET("/integral", {
    char json[512];
    clock_t start = clock();
    double result = simpson_integration(0.0, 5.0, 100000000);
    clock_t end = clock();
    double cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;

    sprintf(json, "{'methodName':'simpson', 'result':%f, 'time':%f}", result, cpu_time_used);
    send_json(json, client_fd);
})
