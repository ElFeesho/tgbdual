#pragma once
#include "link_cable_source.h"
#include <stdint.h>

class tcp_server : public link_cable_source {
public:
	tcp_server();
	uint8_t readByte() override;
	void sendByte(uint8_t data) override;

private:
	int client_socket;
};