#ifndef HTTP_PARSER_H
#define HTTP_PARSER_H

/**
 * @brief HTTP request methods
 */
typedef enum
{
    POST,
    GET,
    PATCH,
    PUT,
    DELETE,
} RequestMethod;

/**
 * @brief HTTP versions
 */
typedef enum
{
    V1_1,
    V1_2,
} HTTPVersion;

/**
 * @brief HTTP parameter structure
 */
typedef struct Param
{
    char *query;
    char *value;
} Param;

/**
 * @brief HTTP request structure
 */
typedef struct
{
    RequestMethod method;
    char *end_point;
    HTTPVersion version;
    char *user_agent;
    Param *Params;
} Request;

/**
 * @brief Read an HTTP request from a client socket
 *
 * @param client_fd Client socket file descriptor
 * @return Request Parsed HTTP request
 */
Request read_request(int client_fd);

/**
 * @brief Extract request data from a raw HTTP request string
 *
 * @param str_request Raw HTTP request string
 * @return Request Parsed HTTP request
 */
Request extract_request(char *str_request);

#endif /* HTTP_PARSER_H */
