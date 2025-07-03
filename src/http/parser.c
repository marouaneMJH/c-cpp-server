#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../../include/http/parser.h"
#include "../../include/server/config.h"

Request extract_request(char *str_request)
{
    Request req;
    req.end_point = NULL;
    req.user_agent = NULL;
    req.Params = NULL;

    // Parse the request line (first line)
    char *line = strtok(str_request, "\n");
    if (line)
    {
        // Parse method
        if (strncmp(line, "GET", 3) == 0)
            req.method = GET;
        else if (strncmp(line, "POST", 4) == 0)
            req.method = POST;
        else if (strncmp(line, "PUT", 3) == 0)
            req.method = PUT;
        else if (strncmp(line, "DELETE", 6) == 0)
            req.method = DELETE;
        else if (strncmp(line, "PATCH", 5) == 0)
            req.method = PATCH;

        // Parse endpoint
        char *endpoint_start = strchr(line, ' ');
        if (endpoint_start)
        {
            endpoint_start++; // Skip the space
            char *endpoint_end = strchr(endpoint_start, ' ');
            if (endpoint_end)
            {
                int length = endpoint_end - endpoint_start;

                // Look for query parameters
                char *query_start = strchr(endpoint_start, '?');
                if (query_start && query_start < endpoint_end)
                {
                    // Extract base endpoint without query parameters
                    int base_length = query_start - endpoint_start;
                    req.end_point = malloc(base_length + 1);
                    if (req.end_point)
                    {
                        strncpy(req.end_point, endpoint_start, base_length);
                        req.end_point[base_length] = '\0';
                    }

                    // Parse query parameters
                    query_start++; // Skip the '?'
                    char *query_string = malloc(endpoint_end - query_start + 1);
                    if (query_string)
                    {
                        strncpy(query_string, query_start, endpoint_end - query_start);
                        query_string[endpoint_end - query_start] = '\0';

                        // Count parameters to allocate array
                        int param_count = 1;
                        for (char *c = query_string; *c; c++)
                            if (*c == '&')
                                param_count++;

                        req.Params = malloc(sizeof(Param) * (param_count + 1)); // +1 for NULL terminator
                        if (req.Params)
                        {
                            char *param_token = strtok(query_string, "&");
                            int i = 0;

                            while (param_token && i < param_count)
                            {
                                char *value_separator = strchr(param_token, '=');
                                if (value_separator)
                                {
                                    *value_separator = '\0'; // Split the string
                                    value_separator++;       // Move to value part

                                    req.Params[i].query = strdup(param_token);
                                    req.Params[i].value = strdup(value_separator);
                                }
                                else
                                {
                                    req.Params[i].query = strdup(param_token);
                                    req.Params[i].value = strdup("");
                                }

                                param_token = strtok(NULL, "&");
                                i++;
                            }

                            // Null terminate the array
                            req.Params[i].query = NULL;
                            req.Params[i].value = NULL;
                        }
                        free(query_string);
                    }
                }
                else
                {
                    // No query parameters
                    req.end_point = malloc(length + 1);
                    if (req.end_point)
                    {
                        strncpy(req.end_point, endpoint_start, length);
                        req.end_point[length] = '\0';
                    }
                }

                // Parse HTTP version
                if (strstr(endpoint_end, "HTTP/1.1"))
                    req.version = V1_1;
                else if (strstr(endpoint_end, "HTTP/1.2"))
                    req.version = V1_2;
            }
        }
    }

    // Parse headers for User-Agent
    line = strtok(NULL, "\n");
    while (line)
    {
        if (strncmp(line, "User-Agent:", 11) == 0)
        {
            char *ua_start = line + 11;
            // Skip leading spaces
            while (*ua_start == ' ')
                ua_start++;

            int length = strlen(ua_start);
            req.user_agent = malloc(length + 1);
            if (req.user_agent)
            {
                strcpy(req.user_agent, ua_start);
                // Remove trailing CR if present
                if (length > 0 && req.user_agent[length - 1] == '\r')
                    req.user_agent[length - 1] = '\0';
            }
            break;
        }
        line = strtok(NULL, "\n");
    }

    return req;
}

Request read_request(int client_fd)
{
    Request empty_request = {0}; // Initialize empty request for error cases

    // Allocate the memory for the user request
    char *req = (char *)malloc(BUFFER_SIZE * sizeof(char));

    // Check for the Allocation
    if (!req)
        return empty_request;

    // Read the Request
    ssize_t bytes_read = read(client_fd, req, BUFFER_SIZE - 1);

    if (bytes_read < 0)
    {
        perror("read failed");
        free(req);
        return empty_request;
    }

    // Null-terminate the string
    req[bytes_read] = '\0';

    // Extract the request
    Request result = extract_request(req);

    // Free the memory
    free(req);

    return result;
}
