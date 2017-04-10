#pragma once

#include <string>

#include <link_cable_source.h>

class tcp_client : public link_cable_source {
   public:
    tcp_client(const std::string &address);

    void sendByte(uint8_t data) override;
    uint8_t readByte() override;

   private:
    int client_socket;
};