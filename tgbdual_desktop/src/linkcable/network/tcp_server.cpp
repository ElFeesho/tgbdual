#include <arpa/inet.h>

#include "tcp_server.h"

tcp_link_cable *create_tcp_server_cable() {
    int net_socket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in net_sockaddr_in;
    net_sockaddr_in.sin_family = AF_INET;
    net_sockaddr_in.sin_port = htons(1338);

    net_sockaddr_in.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(net_socket, (struct sockaddr *)&net_sockaddr_in, sizeof(struct sockaddr_in));
    listen(net_socket, 2);

    struct sockaddr_in client_addr;
    socklen_t size;
    int client_socket = accept(net_socket, (struct sockaddr *)&client_addr, &size);
    return new tcp_link_cable{client_socket};
}
