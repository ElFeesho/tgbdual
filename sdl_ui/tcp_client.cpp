#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "tcp_client.h"

tcp_client::tcp_client(const std::string &addr) {
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in net_sockaddr_in = {0};

    net_sockaddr_in.sin_family = AF_INET;

    net_sockaddr_in.sin_port = htons(1338);

    inet_aton(addr.c_str(), (struct in_addr *)&net_sockaddr_in.sin_addr.s_addr);

    connect(client_socket, (struct sockaddr *)&net_sockaddr_in, sizeof(struct sockaddr_in));
}

void tcp_client::sendByte(uint8_t data) {
    send(client_socket, (void *)&data, 1, 0);
}

uint8_t tcp_client::readByte() {
    uint8_t data = 0;
    recv(client_socket, (void *)&data, 1, 0);
    return data;
}