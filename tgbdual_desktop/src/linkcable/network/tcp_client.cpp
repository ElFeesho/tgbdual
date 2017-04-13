#include <arpa/inet.h>

#include "tcp_client.h"

tcp_link_cable *create_client_cable(const std::string &address) {
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in net_sockaddr_in = {0};
    net_sockaddr_in.sin_family = AF_INET;
    net_sockaddr_in.sin_port = htons(1338);

    inet_aton(address.c_str(), (struct in_addr *)&net_sockaddr_in.sin_addr.s_addr);

    connect(client_socket, (struct sockaddr *)&net_sockaddr_in, sizeof(struct sockaddr_in));
    return new tcp_link_cable{client_socket};
}
