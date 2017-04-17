#include <sys/socket.h>
#include "tcp_link_cable.h"

tcp_link_cable::tcp_link_cable(int tcp_socket) : _tcp_socket{tcp_socket} {
}

uint8_t tcp_link_cable::readByte() {
    uint8_t data = 0;
    recv(_tcp_socket, (void *)&data, 1, 0);
    return data;
}

void tcp_link_cable::sendByte(uint8_t data) {
    send(_tcp_socket, (void *)&data, 1, 0);
}
