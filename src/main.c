#include <sys/socket.h>
#include <stdio.h>
#include "./../includes/socket_setup.h"

int main()
{

    socket_tcp();
    printf("Hello world\n");

    return 0;
}