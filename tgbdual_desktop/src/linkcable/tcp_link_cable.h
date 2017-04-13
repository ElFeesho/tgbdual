#pragma once

#include <link_cable_source.h>

class tcp_link_cable : public link_cable_source {
public:
    tcp_link_cable(int tcp_socket);

    uint8_t readByte() override;

    void sendByte(uint8_t uint8) override;

private:
    int _tcp_socket;
};
