#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "tcp_server.h"

tcp_server::tcp_server() {
	int net_socket = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in net_sockaddr_in = { 0 };

	net_sockaddr_in.sin_family = AF_INET;

	net_sockaddr_in.sin_port = htons(1338);

	net_sockaddr_in.sin_addr.s_addr = htonl(INADDR_ANY);

	bind(net_socket, (struct sockaddr*)&net_sockaddr_in, sizeof(struct sockaddr_in));
	listen(net_socket, 2);

	struct sockaddr_in client_addr = { 0 };
	socklen_t size;
	client_socket = accept(net_socket, (struct sockaddr*)&client_addr, &size);
}

uint8_t tcp_server::readByte()
{
	uint8_t data = 0;
	recv(client_socket, (void*)&data, 1, 0);
	return data;
}

void tcp_server::sendByte(uint8_t data)
{
	send(client_socket, (void*)&data, 1, 0);
}